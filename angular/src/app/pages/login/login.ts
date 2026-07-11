import { Component, ViewChild } from '@angular/core';
import { DxFormModule, DxFormComponent } from 'devextreme-angular';
import { Properties } from 'devextreme/ui/form';
import { HttpClient } from '@angular/common/http';
import { Router } from '@angular/router';
import notify from 'devextreme/ui/notify';
import { Auth } from '../../services/auth';
@Component({
  selector: 'app-login',
  imports: [DxFormModule],
  templateUrl: './login.html',
  styleUrl: './login.scss',
})
export class Login {
  @ViewChild(DxFormComponent, { static: false }) dxForm?: DxFormComponent;
  constructor(
    private http: HttpClient,
    private router: Router,
    private auth: Auth
  ) { }

  formdata = {
    tenant_name: '',
    username: '',
    password: '',
  };

  formItems: DxFormComponent['items'] = [
    {
      dataField: 'tenant_name',
      label: { text: '組織名稱' },
      editorOptions: {
        placeholder: '請輸入組織名稱',
      },
      validationRules: [
        { type: 'required', message: '組織名稱為必填' },
      ],
    },
    {
      dataField: 'username',
      label: { text: '使用者名稱' },
      editorOptions: {
        placeholder: '請輸入使用者名稱',
      },
      validationRules: [
        { type: 'required', message: '使用者名稱為必填' },
        { type: 'stringLength', min: 3, max: 50, message: '3-50 字元' },
      ],
    },
    {
      dataField: 'password',
      label: { text: '密碼' },
      editorOptions: {
        placeholder: '請輸入密碼',
        mode: 'password',
      },
      validationRules: [
        { type: 'required', message: '密碼為必填' },
        { type: 'stringLength', min: 6, max: 255, message: '6-255 字元' },
      ],
    },
    {
      itemType: 'button',
      horizontalAlignment: 'center',
      buttonOptions: {
        text: '登入',
        type: 'success',
        onClick: () => this.onSubmit(),
      },
    },
  ];

  formOptions: Properties = {
    colCount: 1,
    items: this.formItems,
  };

  onSubmit() {
    if (this.dxForm) {
      const result = this.dxForm.instance.validate();
      if (!result.isValid) {
        notify('請完整填寫所有必填欄位', 'error', 2000);
        return;
      }
    }

    this.http.post('/login', this.formdata).subscribe({
      next: (response: any) => {
        // 預期 response 會有 token, tenant_id, tenant_name, message
        if (response && response.token && response.tenant_id && response.tenant_name) {
          this.auth.setAuthInfo({
            user_id: response.user_id,
            user_name: response.user_name,
            token: response.token,
            tenant_id: response.tenant_id,
            tenant_name: response.tenant_name,
            team_id: response.team_id,
            team_name: response.team_name,
          });


          this.router.navigate(['/']);
        }
        notify('登入成功！', 'success', 2000);
        this.router.navigate(['/']);
      },
      error: (error) => {
        notify('登入失敗：' + (error.error?.message || '請稍後再試'), 'error', 3000);
        console.error('登入錯誤:', error);
      }
    });
  }
}