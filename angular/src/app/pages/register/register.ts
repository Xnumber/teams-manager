import { Component, ViewChild } from '@angular/core';
import { DxFormModule, DxFormComponent } from 'devextreme-angular';

import { FieldDataChangedEvent, Properties } from 'devextreme/ui/form';
import { HttpClient } from '@angular/common/http';
import { Router } from '@angular/router';
import notify from 'devextreme/ui/notify';
import { CustomStore, DataSourceOptions } from 'devextreme/common/data';
import { lastValueFrom } from 'rxjs';
import { Team } from '../team/team';
import { FocusInEvent } from 'devextreme/ui/select_box';
import { ChangeEvent } from 'devextreme/ui/text_box';

@Component({
  selector: 'app-register',
  imports: [DxFormModule],
  templateUrl: './register.html',
  styleUrl: './register.scss',
})
export class Register {
  teamsDataSource: DataSourceOptions;
  tenantNameParam: string = '';
  formOptions: Properties;
  formItems: DxFormComponent['items'];
  formdata: Record<string, any>;
  @ViewChild(DxFormComponent, { static: false }) dxForm?: DxFormComponent;
  constructor(
    private http: HttpClient,
    private router: Router
  ) {
    this.teamsDataSource = {
      store: new CustomStore({
        load: () => {
          return lastValueFrom(
            this.http.get<ApiResponse<{ id: string; name: string }[]>>("/teams", { params: { tenant_name: this.tenantNameParam } })
          ).then(res => {
            if (res && Array.isArray(res.data)) {
              return [...res.data.map(t => ({ id: t.id, name: t.name }))];
            }
            return [];
          });
        }
      })
    };


    this.formdata = {
      tenant_name: '',
      username: '',
      email: '',
      password: '',
      confirmPassword: '',
    };



    this.formItems = [
      {
        dataField: 'tenant_name',
        label: { text: '組織名稱' },
        editorOptions: {
          placeholder: '請輸入組織名稱',
          onChange: (e: ChangeEvent) => {
            this.formdata['team_id'] = null;
          }
        },
        validationRules: [
          { type: 'required', message: '組織名稱為必填' },
        ],
      },
      {
        dataField: 'team_id',
        label: { text: '團隊' },
        editorType: 'dxSelectBox',
        editorOptions: {
          placeholder: '請選擇團隊',
          dataSource: this.teamsDataSource,
          displayExpr: 'name',
          valueExpr: 'id',
          onFocusIn: (e: FocusInEvent) => {
            if(!this.formdata['tenant_name']) {
              e.component.instance().option('dataSource', []); // 清空團隊列表
              return
            } else {
              e.component.instance().option('dataSource', this.teamsDataSource);
              e.component.instance().getDataSource().reload(); // 每次獲取焦點時重新載入團隊列表
            }
          }
        },
        validationRules: [
          { type: 'required', message: '團隊為必填' },
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
        dataField: 'email',
        label: { text: '電子郵件' },
        editorOptions: {
          placeholder: '請輸入電子郵件',
        },
        validationRules: [
          { type: 'required', message: '電子郵件為必填' },
          { type: 'email', message: '請輸入正確的電子郵件格式' },
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
        dataField: 'confirmPassword',
        label: { text: '確認密碼' },
        editorOptions: {
          placeholder: '請再次輸入密碼',
          mode: 'password',
        },
        validationRules: [
          { type: 'required', message: '請再次輸入密碼' },
          {
            type: 'compare',
            comparisonTarget: () => this.formdata['password'],
            message: '兩次密碼輸入不一致',
          },
        ],
      },
      {
        itemType: 'button',
        horizontalAlignment: 'center',
        buttonOptions: {
          text: '註冊',
          type: 'success',
          onClick: () => this.onSubmit(),
        },
      },
    ];
    this.formOptions = {
      colCount: 1,
      items: this.formItems,
    };
  }





  onSubmit() {
    if (this.dxForm) {
      const result = this.dxForm.instance.validate();
      if (!result.isValid) {
        notify('請完整填寫所有必填欄位', 'error', 2000);
        return;
      }
    }

    this.http.post('/register', this.formdata).subscribe({
      next: (response) => {
        notify('註冊成功！', 'success', 2000);
        this.router.navigate(['/login']);
      },
      error: (error) => {
        notify('註冊失敗：' + (error.error?.message || '請稍後再試'), 'error', 3000);
        console.error('註冊錯誤:', error);
      }
    });
  }

  @ViewChild(DxFormComponent, { static: false }) formComponent?: DxFormComponent;

  onFieldDataChanged(e: FieldDataChangedEvent) {
    // 當 tenant_name 欄位改變時，更新 tenantNameParam
    if (e.dataField === 'tenant_name') {
      this.tenantNameParam = e.value;
      // this.teamsDataSource = {
      //   store: new CustomStore({
      //     load: () => {
      //       return lastValueFrom(
      //         this.http.get<ApiResponse<{ id: string; name: string }[]>>("/teams", { params: { tenant_name: this.tenantNameParam } })
      //       ).then(res => {
      //         if (res && Array.isArray(res.data)) {
      //           return [...res.data.map(t => ({ id: t.id, name: t.name }))];
      //         }
      //         return [];
      //       });
      //     }
      //   })
      // };
      // this.formComponent?.instance.getEditor('team_id')?.reset();
      // this.formComponent?.instance.getEditor('team_id')?.repaint();
      // 重新觸發 teamsDataSource reload
      // const teamItem = this.formItems.find(item => item.dataField === 'team_id');
      // if (teamItem && teamItem.editorOptions) {
      //   teamItem.editorOptions.dataSource = this.teamsDataSource;
      // }
    }
  }
}
