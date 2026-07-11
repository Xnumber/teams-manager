import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom, Observable, of } from 'rxjs';
import CustomStore from 'devextreme/data/custom_store';
import { DataSource, LoadOptions } from 'devextreme-angular/common/data';
import { JBTask, RawTask } from '../pages/tasks/typing';
import { paramBuilder } from '../utils/paramBuilder';

interface SetTaskCompletedResponse {
  result: string;
  task?: unknown;
  message?: string;
}

@Injectable({
  providedIn: 'root',
})
export class Task {
  constructor(private http: HttpClient) { }

  setTaskcompleted(taskId: string): Observable<SetTaskCompletedResponse> {
    return this.http.put<SetTaskCompletedResponse>(`/set-task-completed/${taskId}`, {});
  }


  createTasksLookupDataSource = () => {
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<any>(`/tasks/${key}`)).then(res => {
            if (res && res.tasks) {
              return { id: res.tasks.id, name: res.tasks.name };
            }
            throw new Error('Task not found');
          });
        },
        load: (loadOptions: LoadOptions) => {
          const filter = loadOptions.searchValue
          const params: Record<string, any> = {}
          if (filter) {
            params['filter'] = filter;
          }
          return lastValueFrom(this.http.get<any>("/tasks", { params })).then(res => {
            if (res && Array.isArray(res.tasks)) {
              return res.tasks.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      })
    };
  }

  createTasksDependencyLookupDataSource = (id?: string) => {
    return {
      store: new CustomStore({
        byKey: (key: string) => {

          if(id) {
            return lastValueFrom(this.http.get<any>(`/tasks/${id}/dependency`)).then(res => {
              if (res && res.data) {
                return { id: res.data.id, name: res.data.name };
              }
              throw new Error('Task not found');
            });
          } else {
            return lastValueFrom(of(null))
          }

        },
        load: (loadOptions: LoadOptions) => {
          const filter = loadOptions.searchValue
          const params: Record<string, any> = {}
          if (filter) {
            params['filter'] = filter;
          }
          return lastValueFrom(this.http.get<any>("/tasks", { params })).then(res => {
            if (res && Array.isArray(res.tasks)) {
              return res.tasks.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      })
    };
  }

  // createTasksDataSource(milestoneId: string): DataSource<JBTask> {
  createTasksDataSource(): DataSource<JBTask> {
      return new DataSource({
        load: (loadOptions: LoadOptions<JBTask>) => {
          let url = '/tasks-filter';
          const params: any = paramBuilder(loadOptions);

          return lastValueFrom(this.http.get<{ count: number; result: string; data: RawTask[] }>(url, { params }))
            .then(data => {
              if (data?.result === 'ok' && Array.isArray(data.data)) {
  
                return {
                  data: data.data.map(task => ({
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
      });
    }



  reprioritizeTask(taskId: string, milestoneId: string, newPriority: number): Promise<void> {
    return lastValueFrom(
      this.http.put<{ result: string; message?: string }>(`/task/reprioritize`, {
        task_id: taskId,
        milestone_id: milestoneId,
        priority: newPriority,
      })
    ).then((res) => {
      if (res?.result !== 'ok') {
        throw new Error(res?.message || 'Reprioritize failed');
      }
    });
  }
}