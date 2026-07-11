
import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { catchError, lastValueFrom } from 'rxjs';
import { LoadOptions } from 'devextreme/data';
import { JBTask, RawTask } from '../tasks/typing';

@Injectable({
  providedIn: 'root',
})
export class TasksDataSourceService {
  selectedCreatorId: string | null = null;
  constructor(private http: HttpClient) { }

  createTasksFilterDataSource(milestoneId: string): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        let url = '/tasks-filter';
        const params: any = { milestone_id: milestoneId };
        if (this.selectedCreatorId) {
          // params.creator_id = this.selectedCreatorId;
        }
        return lastValueFrom(this.http.get<ApiResponse<JBTask>>(url, { params }))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {

              return {
                data: data.data.map(task => ({
                  ...task,
                  executor_ids: task.executors?.map((e: any) => e.user_id) || [],
                  mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
                }) as JBTask),
                totalCount: data.data.length
              }
            }
            return [];
          });
      },
      update: (key: any, values: Record<string, any>) => {
        // key 為要更新的資料的 key，values 為 DevExtreme 編輯後的資料
        return lastValueFrom(this.http.put<ApiResponse<RawTask>>(`/tasks/${key.id}`, {
          ...key,
          ...values
        }))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return {
                ...res.data,
              } as JBTask;
            }
            throw new Error(res?.result || 'Update failed');
          });
      },
      insert: (values: Record<string, any>) => {
        // values 為 DevExtreme 新增資料
        return lastValueFrom(this.http.post<ApiResponse<RawTask>>("/tasks", values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return {
                ...res.data
              } as JBTask;
            }
            throw new Error(res?.result || 'Create failed');
          });
      }
    });
  }
}