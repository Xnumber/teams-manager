// import { Injectable } from '@angular/core';
// import DataSource from 'devextreme/data/data_source';
// import CustomStore from 'devextreme/data/custom_store';

// export interface WorkRequestFormRow {
//   id: string;
//   requester_name: string;
//   tenant_name: string;
//   project_name?: string;
//   title: string;
//   description?: string;
//   scheduled_start_date: string;
//   scheduled_end_date: string;
//   estimated_workdays: number;
//   priority: number;
//   approval_status: string;
//   created_at?: string;
// }

// @Injectable({
//   providedIn: 'root'
// })
// export class WorkRequestFormDataSourceService {
//   private rows: WorkRequestFormRow[] = [
//     {
//       id: 'wrf-001',
//       requester_name: '王小明',
//       tenant_name: '總公司',
//       project_name: 'Teams Manager',
//       title: '建立工作需求單功能',
//       description: '可建立並管理工作需求單，後續可轉工作。',
//       scheduled_start_date: '2026-05-26',
//       scheduled_end_date: '2026-05-30',
//       estimated_workdays: 3,
//       priority: 2,
//       approval_status: 'pending',
//       created_at: '2026-05-24T10:00:00'
//     },
//     {
//       id: 'wrf-002',
//       requester_name: '陳小華',
//       tenant_name: '研發中心',
//       project_name: 'Internal Portal',
//       title: '需求單查詢篩選優化',
//       description: '加入關鍵字搜尋與日期區間。',
//       scheduled_start_date: '2026-05-27',
//       scheduled_end_date: '2026-06-02',
//       estimated_workdays: 4,
//       priority: 1,
//       approval_status: 'approved',
//       created_at: '2026-05-24T11:30:00'
//     }
//   ];

//   createWorkRequestFormDataSource(): DataSource<WorkRequestFormRow> {
//     const store = new CustomStore<WorkRequestFormRow, string>({
//       key: 'id',
//       load: () => {
//         return Promise.resolve([...this.rows]);
//       },
//       insert: (values: Record<string, unknown>) => {
//         const now = new Date().toISOString();
//         const newRow: WorkRequestFormRow = {
//           id: `wrf-${Math.random().toString(36).slice(2, 10)}`,
//           requester_name: String(values['requester_name'] ?? ''),
//           tenant_name: String(values['tenant_name'] ?? ''),
//           project_name: values['project_name'] ? String(values['project_name']) : undefined,
//           title: String(values['title'] ?? ''),
//           description: values['description'] ? String(values['description']) : undefined,
//           scheduled_start_date: String(values['scheduled_start_date'] ?? ''),
//           scheduled_end_date: String(values['scheduled_end_date'] ?? ''),
//           estimated_workdays: Number(values['estimated_workdays'] ?? 0),
//           priority: Number(values['priority'] ?? 0),
//           approval_status: String(values['approval_status'] ?? 'pending'),
//           created_at: now
//         };

//         this.rows = [newRow, ...this.rows];
//         return Promise.resolve(newRow);
//       },
//       update: (key: string, values: Record<string, unknown>) => {
//         let updatedRow: WorkRequestFormRow | null = null;

//         this.rows = this.rows.map((row) => {
//           if (row.id !== key) {
//             return row;
//           }
//           updatedRow = {
//             ...row,
//             ...values,
//             estimated_workdays: Number(values['estimated_workdays'] ?? row.estimated_workdays ?? 0),
//             priority: Number(values['priority'] ?? row.priority ?? 0)
//           };
//           return updatedRow;
//         });

//         if (!updatedRow) {
//           return Promise.reject(new Error('Update failed'));
//         }

//         return Promise.resolve(updatedRow);
//       },
//       remove: (key: string) => {
//         const removed = this.rows.find((row) => row.id === key);
//         this.rows = this.rows.filter((row) => row.id !== key);
//         return Promise.resolve(removed ?? key);
//       }
//     });

//     return new DataSource({
//       store
//     });
//   }
// }
