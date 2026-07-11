// import { Component } from '@angular/core';
// import { DxDataGridModule } from 'devextreme-angular';
// import DataSource from 'devextreme/data/data_source';
// import { Column } from 'devextreme/ui/data_grid';
// import { WorkRequestFormDataSourceService } from './work-request-form-data-source.service';

// @Component({
//   selector: 'app-work-request-form',
//   standalone: true,
//   imports: [DxDataGridModule],
//   templateUrl: './work-request-form.html',
//   styleUrl: './work-request-form.scss',
//   providers: [WorkRequestFormDataSourceService]
// })
// export class WorkRequestForm {
//   workRequestFormDataSource: DataSource<any>;
//   columns: Column[] = [];

//   constructor(
//     private workRequestFormDataSourceService: WorkRequestFormDataSourceService
//   ) {
//     this.workRequestFormDataSource = this.workRequestFormDataSourceService.createWorkRequestFormDataSource();
//     this.columns = [
//       { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
//       { dataField: 'requester_name', caption: '提出人', validationRules: [{ type: 'required' }] },
//       { dataField: 'tenant_name', caption: '租戶名稱', validationRules: [{ type: 'required' }] },
//       { dataField: 'project_name', caption: '專案名稱' },
//       { dataField: 'title', caption: '需求標題', validationRules: [{ type: 'required' }] },
//       { dataField: 'description', caption: '需求描述' },
//       { dataField: 'scheduled_start_date', caption: '預計開始日', dataType: 'date', validationRules: [{ type: 'required' }] },
//       { dataField: 'scheduled_end_date', caption: '預計結束日', dataType: 'date', validationRules: [{ type: 'required' }] },
//       { dataField: 'estimated_workdays', caption: '預估工作天數', dataType: 'number' },
//       { dataField: 'priority', caption: '優先度', dataType: 'number' },
//       { dataField: 'approval_status', caption: '審核狀態' },
//       { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false, formItem: { visible: false } }
//     ];
//   }

// }
