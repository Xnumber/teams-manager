
import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { catchError, lastValueFrom } from 'rxjs';
import { JBTask, RawTask } from './typing';
import { LoadOptions } from 'devextreme/data';

@Injectable({
  providedIn: 'root',
})
export class TasksDataSourceService {
  selectedCreatorId: string | null = null;
  constructor(private http: HttpClient) { }

  createAllTasksDataSource(): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        let url = '/tasks';
        const params: any = {};
        if (this.selectedCreatorId) {
          // params.creator_id = this.selectedCreatorId;
        }
        return lastValueFrom(this.http.get<{ count: number; result: string; tasks: RawTask[] }>(url, { params }))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.tasks)) {

              return {
                data: data.tasks.map(task => ({
                  ...task,
                  executor_ids: task.executors?.map((e: any) => e.user_id) || [],
                  mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
                }) as JBTask),
                totalCount: data.count
              }
            }
            return [];
          });
      },
      update: (key: any, values: Record<string, any>) => {
        // key 為要更新的資料的 key，values 為 DevExtreme 編輯後的資料
        console.log('Updating task with key', key, 'and values', values);
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
      },
      // 可加 update/remove 方法
    });
  }

  createThisWeekTasksDataSource(): DataSource<JBTask> {

    // Optionally implement insert/update/remove for CRUD
    return new DataSource({
      filter: [],
      load: (loadOptions: LoadOptions<JBTask>) => {
        let url = '/this-week-tasks';
        const params: any = {};
        console.log('Filtering by creator_id', loadOptions);
        if (loadOptions.filter && Array.isArray(loadOptions.filter) && loadOptions.filter.length >= 2) {
          params.executor_id = loadOptions.filter[0][2]; // 假設 filter 是 ["executor_id", "=", "some_id"]
          params.mentor_id = loadOptions.filter[1][2]; // 假設 filter 是 ["mentor_id", "=", "some_id"]
          // params.creator_id = loadOptions.searchExpr[0];
        }
        return lastValueFrom(this.http.get<{ count: number; result: string; tasks: RawTask[] }>(url, { params }))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.tasks)) {
              return {
                data: data.tasks.map(task => ({
                  ...task,
                  executor_ids: task.executors?.map((e: any) => e.user_id) || [],
                  mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
                  // executors: task.participants ? JSON.parse(task.participants).filter((p: any) => p.type === 'executor').map((p: any) => p.name).join(', ') : '',
                  // mentors: task.participants ? JSON.parse(task.participants).filter((p: any) => p.type === 'mentor').map((p: any) => p.name).join(', ') : '',
                }) as JBTask),
                totalCount: data.count
              }
            }
            return [];
          });
      },
      insert: (values: any) => {
        // values 為 DevExtreme 新增資料
        return lastValueFrom(this.http.post<{ result: string; data: RawTask }>("/tasks", values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return {
                ...res.data,
                executor_ids: res.data.executors?.map((e: any) => e.user_id) || [],
                mentor_ids: res.data.mentors?.map((e: any) => e.user_id) || [],
                // executors: res.task.participants ? JSON.parse(res.task.participants).filter((p: any) => p.type === 'executor').map((p: any) => p.name).join(', ') : '',
                // mentors: res.task.participants ? JSON.parse(res.task.participants).filter((p: any) => p.type === 'mentor').map((p: any) => p.name).join(', ') : '',
              } as JBTask;
            }
            throw new Error(res?.result || 'Create failed');
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
                executor_ids: res.data.executors?.map((e: any) => e.user_id) || [],
                mentor_ids: res.data.mentors?.map((e: any) => e.user_id) || [],
              } as JBTask;
            }
            throw new Error(res?.result || 'Update failed');
          })
      },
      // 可加 update/remove 方法
    });


  }

  createNextWeekTasksDataSource(): DataSource<JBTask> {

    // Optionally implement insert/update/remove for CRUD
    return new DataSource({
      load: (loadOptions) => {
        console.log('Next week tasks loading with options', loadOptions);
        let url = '/next-week-tasks';
        const params: any = {};
        if (loadOptions.filter && Array.isArray(loadOptions.filter) && loadOptions.filter.length >= 2) {
          params.executor_id = loadOptions.filter[0][2]; // 假設 filter 是 ["creator_id", "=", "some_id"]
          params.mentor_id = loadOptions.filter[1][2]; // 假設 filter 是 ["creator_id", "=", "some_id"]
          // params.creator_id = loadOptions.searchExpr[0];
        }
        return lastValueFrom(this.http.get<{ count: number; result: string; tasks: RawTask[] }>(url, { params })
          .pipe(catchError(err => {
            console.error('Error loading next week tasks', err);
            return [];
          }))
        )
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.tasks)) {
              return {
                data: data.tasks.map(task => ({
                  ...task,
                  executor_ids: task.executors?.map((e: any) => e.user_id) || [],
                  mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
                }) as JBTask),
                totalCount: data.count
              }
            }
            return [];
          });
      },
      insert: (values: any) => {
        // values 為 DevExtreme 新增資料
        return lastValueFrom(this.http.post<{ result: string; data: RawTask }>("/tasks", values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return {
                ...res.data,
                // executors: res.task.participants ? JSON.parse(res.task.participants).filter((p: any) => p.type === 'executor').map((p: any) => p.name).join(', ') : '',
                // mentors: res.task.participants ? JSON.parse(res.task.participants).filter((p: any) => p.type === 'mentor').map((p: any) => p.name).join(', ') : '',
              } as JBTask;
            }
            throw new Error(res?.result || 'Create failed');
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
      // 可加 update/remove 方法
    });
  }
}
