import { Component, signal } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular';
import { Column } from 'devextreme/ui/data_grid';
import { RejectForm } from './reject-form/reject-form';


@Component({
  selector: 'app-requirement-confirmation-histories',
  imports: [DxDataGridModule, RejectForm],
  templateUrl: './requirement-confirmation-histories.html',
  styleUrl: './requirement-confirmation-histories.scss',
})
export class RequirementConfirmationHistories {
  showRejectForm = signal(false);

  closeRejectForm = () => {
    this.showRejectForm.set(false);
    this.selectedConfirmRequestId.set('');
  }

  selectedConfirmRequestId = signal<string>('');
  data = [{ a: 1, b: 2 }, { a: 3, b: 4 }]; // 模擬數據，實際應從後端獲取
  columns: Column[] = [
    { dataField: 'id', caption: '編號', visible: false },
    // status 'pending', 'accepted' 'rejected', 'executor_confirmed', 'closed' 等等
    { dataField: 'status', caption: '狀態', dataType: 'string' },
    { dataField: 'task_id', caption: '編號' },
    { dataField: 'task_name', caption: '名稱' },
    { dataField: 'task_description', caption: '工作描述' },
    { dataField: 'executor_ids', caption: '執行人員' },
    { dataField: 'acceptor_ids', caption: '確認人員' },
    { dataField: 'requested_at', caption: '確認請求時間', dataType: 'datetime' },
    { dataField: 'accepted', caption: '通過' },
    { dataField: 'reject_type_id', caption: '退回類型' },
    { dataField: 'reject_reason', caption: '退回原因' },
    { dataField: 'executor_response_type_id', caption: "執行者回應類型" },
    { dataField: 'acceptor_comment', caption: '確認意見' },
    { dataField: 'executor_comment', caption: '開發回覆' },
    { dataField: 'reviewed_at', caption: '審核時間', dataType: 'datetime' },
    { dataField: 'created_at', caption: '建立時間', dataType: 'datetime' },
    { dataField: 'updated_at', caption: '更新時間', dataType: 'datetime' },
    {
      type: 'buttons', 
      buttons: [
        {
          text: '查看詳情',
          onClick: (e) => {
            const data = e.row?.data;
            // 在這裡處理查看詳情的邏輯，例如導航到詳情頁面或顯示彈窗
            console.log('查看詳情', data);
          }
        },
        {
          text: '確認',
          onClick: (e) => {
            const data = e.row?.data;
            // 在這裡處理請求確認的邏輯，例如發送請求到後端或顯示確認對話框
            console.log('請求確認', data);
          }
        },
        {
          text: '拒絕',
          onClick: (e) => {
            const data = e.row?.data;
            // 在這裡處理請求拒絕的邏輯，例如發送請求到後端或顯示拒絕對話框
            console.log('請求拒絕', data);
            this.selectedConfirmRequestId.set(data.id);
            this.showRejectForm.set(true);
          }
        },
      ]
    }
  ];
}