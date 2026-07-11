import { HttpClient } from '@angular/common/http';
import { Component, inject, input, output } from '@angular/core';
import { DxButtonModule, DxFormModule, DxPopupModule } from 'devextreme-angular';
import { Properties } from 'devextreme/ui/form';

@Component({
  selector: 'app-reject-form',
  imports: [
    DxFormModule,
    DxPopupModule,
    DxButtonModule
  ],
  templateUrl: './reject-form.html',
  styleUrl: './reject-form.scss',
})
export class RejectForm {
  formData = {
    reject_type_id: null,
    reject_reason: '',
  };
  visible = input(false);
  http = inject(HttpClient);
  taskAcceptanceRecordId = input<string>('');
  taskName = input<string>('-');
  taskDescription = input<string>('-');
  cancelled = output();
  rejected = output();
  rejectTypeDataSource = [
    { id: 1, name: '需求不清楚' },
    { id: 2, name: '需求有誤' },
    { id: 3, name: '其他' },
  ];

  formProperties: Properties = {
    items: [
      {
        dataField: 'reject_type_id',
        label: { text: '退回類型' },
        editorType: 'dxSelectBox',
        editorOptions: {
          items: [
            { id: 1, name: '需求不清楚' },
            { id: 2, name: '需求有誤' },
            { id: 3, name: '其他' },
          ],
          displayExpr: 'name',
          valueExpr: 'id',
        }
      },
      {
        dataField: 'reject_reason',
        label: { text: '退回原因' },
        editorType: 'dxTextArea',
      }
    ]
  }
  cancel = () => {
    this.cancelled.emit();
  }
  reject = () => {
    this.rejected.emit();
    this.http.post('/requirement-confirmation/reject', {
      task_acceptance_record_id: this.taskAcceptanceRecordId(),
      reject_type_id: this.formData.reject_type_id,
      reject_reason: this.formData.reject_reason,
    }).subscribe({
      next: () => {
        // 這裡可以顯示成功訊息，然後關閉彈窗
        this.cancelled.emit();
      },
      error: (err) => {
        // 這裡可以顯示錯誤訊息
        console.error('拒絕請求失敗', err);
      }
    });
  }
}