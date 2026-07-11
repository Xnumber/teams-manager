
import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { JBTask, RawTask } from '../../tasks/typing';

@Injectable()
export class TasksDataSourceService {
  constructor(private http: HttpClient) { }
  createInCompletedTasksThisMonthDataSource(userId: string): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        let url = '/completed-this-month-tasks';
        const params: any = {};
        params.executor_id = userId; // 假設 filter 是 ["executor_id", "=", "some_id"]
        params.mentor_id = userId; // 假設 filter 是 ["mentor_id", "=", "some_id"]
        
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
            return {
              data: [],
              totalCount: 0
            };
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
      },
      // 可加 update/remove 方法
    });
  }
  createInCompletedTasksDataSource(userId: string): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        let url = '/incomplete-tasks';
        const params: any = {};
        params.executor_id = userId; // 假設 filter 是 ["executor_id", "=", "some_id"]
        params.mentor_id = userId; // 假設 filter 是 ["mentor_id", "=", "some_id"]
        
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
            return {
              data: [],
              totalCount: 0
            };
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
      },
      // 可加 update/remove 方法
    });
  }
  createCompletedTasksDataSource(userId: string): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        let url = '/completed-tasks';
        const params: any = {};
        params.executor_id = userId; // 假設 filter 是 ["executor_id", "=", "some_id"]
        params.mentor_id = userId; // 假設 filter 是 ["mentor_id", "=", "some_id"]
        
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
            return {
              data: [],
              totalCount: 0
            };
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
      },
      // 可加 update/remove 方法
    });
  }

  /**
   * 取得本月參與的專案的工作數量的datasource
   */
  createParticipatingProjectThisMonthDataSource(userId: string): DataSource<{ project_id: string; project_name: string; count: number; }> {
    return new DataSource<{ project_id: string; project_name: string; count: number; }>({
      load: () => {
        const url = '/participating-projects-this-month';
        const params: any = { user_id: userId };

        return lastValueFrom<{ count: number; result: string; data: Array<{ project_id: string; project_name: string; task_count: number }>; }>(this.http.get<{
          count: number;
          result: string;
          data: Array<{ project_id: string; project_name: string; task_count: number }>;
        }>(url, { params }))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return {
                data: data.data.map(project => ({
                  project_id: project.project_id,
                  project_name: project.project_name,
                  count: project.task_count,
                })),
                totalCount: data.count
              };
            }

            return {
              data: [],
              totalCount: 0
            };
          });
      },
    });
  }

  /**
   * 取得指定月份參與的專案的工作數量的datasource
   */
  createParticipatingProjectsByMonthDataSource(userId: string, month: string): DataSource<{ project_id: string; project_name: string; count: number; }> {
    return new DataSource<{ project_id: string; project_name: string; count: number; }>({
      load: () => {
        const url = '/participating-projects-by-month';
        const params: any = {
          user_id: userId,
          month,
        };

        return lastValueFrom<{ count: number; result: string; data: Array<{ project_id: string; project_name: string; task_count: number }>; month: string; }>(this.http.get<{
          count: number;
          result: string;
          data: Array<{ project_id: string; project_name: string; task_count: number }>;
          month: string;
        }>(url, { params }))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return {
                data: data.data.map(project => ({
                  project_id: project.project_id,
                  project_name: project.project_name,
                  task_count: project.task_count,
                })),
                totalCount: data.count
              };
            }

            return {
              data: [],
              totalCount: 0
            };
          });
      },
    });
  }
  /**
   * 取得指定月份完成的工作數量的datasource
   */
  createCompletedTasksByMonthDataSource(userId: string, month: string): DataSource<JBTask> {
    return new DataSource<JBTask>({
      load: () => {
        const url = '/task-completed-by-month';
        const params: any = {
          user_id: userId,
          month,
        };

        return lastValueFrom<{ count: number; result: string; data: Array<{ project_id: string; project_name: string; task_count: number }>; month: string; }>(this.http.get<{
          count: number;
          result: string;
          data: Array<{ project_id: string; project_name: string; task_count: number }>;
          month: string;
        }>(url, { params }))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return {
                data: data.data,
                totalCount: data.count
              };
            }

            return {
              data: [],
              totalCount: 0
            };
          });
      },
    });
  }
}
