import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { TaskType } from './typing';

interface ApiResponse<T> {
  result: string;
  data?: T;
  task_types?: T[];
  count?: number;
  message?: string;
}

@Injectable({
  providedIn: 'root',
})
export class TaskTypesDataSourceService {
  constructor(private http: HttpClient) {}

  createTaskTypesDataSource(): DataSource<TaskType> {
    return new DataSource({
      key: 'id',
      load: () => {
        return lastValueFrom(
          this.http.get<ApiResponse<TaskType>>('/task-types')
        ).then(data => {
          if (data?.result === 'ok' && Array.isArray(data.task_types)) {
            return data.task_types;
          }
          return [];
        });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(
          this.http.post<ApiResponse<TaskType>>('/task-types', values)
        ).then(res => {
          if (res?.result === 'ok' && res.data) {
            return res.data;
          }
          throw new Error(res?.message || 'Create failed');
        });
      },
      update: (key: string, values: Record<string, any>) => {
        return lastValueFrom(
          this.http.put<ApiResponse<TaskType>>(`/task-types/${key}`, values)
        ).then(res => {
          if (res?.result === 'ok' && res.data) {
            // 後端返回陣列，取第一個元素
            const data = Array.isArray(res.data) ? res.data[0] : res.data;
            return data;
          }
          throw new Error(res?.message || 'Update failed');
        });
      },
      remove: (key: string) => {
        return lastValueFrom(
          this.http.delete<ApiResponse<never>>(`/task-types/${key}`)
        ).then(res => {
          if (res?.result === 'ok') {
            return;
          }
          throw new Error(res?.message || 'Delete failed');
        });
      },
    });
  }
}
