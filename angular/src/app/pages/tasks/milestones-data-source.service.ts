
import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { Milestone } from '../../types/milestone';
import CustomStore from 'devextreme/data/custom_store';

@Injectable({
  providedIn: 'root',
})
export class MilestonesDataSourceService {
  selectedCreatorId: string | null = null;
  constructor(private http: HttpClient) { }



  createMilestonesDataSource() {
    let url = '/milestones';
    const params: any = {};
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<ApiResponse<Milestone>>(url, { params })).then(res => {
            if (res && res.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Milestone not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<ApiResponse<Milestone>>(url, { params })).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      })
    };
  }

  // createMilestonesDataSource(): DataSource<Milestone> {
  //   return new DataSource({
  //     load: () => {
  //       let url = '/milestones';
  //       const params: any = {};
  //       if (this.selectedCreatorId) {
  //         // params.creator_id = this.selectedCreatorId;
  //       }
  //       return lastValueFrom(this.http.get<ApiResponse<Milestone>>(url, { params }))
  //         .then(data => {
  //           if (data?.result === 'ok' && Array.isArray(data.data)) {

  //             return {
  //               data: data.data.map(task => ({
  //                 ...task,
  //                 executor_ids: task.executors?.map((e: any) => e.user_id) || [],
  //                 mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
  //               }) as Milestone),
  //               totalCount: data.data.length
  //             }
  //           }
  //           return [];
  //         });
  //     },
  //     update: (key: any, values: Record<string, any>) => {
  //       // key 為要更新的資料的 key，values 為 DevExtreme 編輯後的資料
  //       return lastValueFrom(this.http.put<ApiResponse<Milestone>>(`/tasks/${key.id}`, {
  //         ...key,
  //         ...values
  //       }))
  //         .then(res => {
  //           if (res?.result === 'ok' && res.data) {
  //             return {
  //               ...res.data,
  //             } as Milestone;
  //           }
  //           throw new Error(res?.result || 'Update failed');
  //         });
  //     },
  //     insert: (values: Record<string, any>) => {
  //       // values 為 DevExtreme 新增資料
  //       return lastValueFrom(this.http.post<ApiResponse<Milestone>>("/tasks", values))
  //         .then(res => {
  //           if (res?.result === 'ok' && res.data) {
  //             return {
  //               ...res.data
  //             } as Milestone;
  //           }
  //           throw new Error(res?.result || 'Create failed');
  //         });
  //     },
  //     // 可加 update/remove 方法
  //   });
  // }
}
