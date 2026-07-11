import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { CustomStore, DataSourceOptions } from 'devextreme/common/data';

@Injectable()
export class TaskLookupDataSourceService {
  constructor(private http: HttpClient) {}

  createUserDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key) => {
          return lastValueFrom(this.http.get<ApiResponse<any>>(`/user/${key}`)).then(res => {
            if (res?.data) {
              return { id: res.data.id, name: res.data.username };
            }
            throw new Error('User not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<ApiResponse<any[]>>('/user')).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map(u => ({ id: u.id, name: u.username }));
            }
            return [];
          });
        }
      })
    };
  }

  createProjectDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key) => {
          return lastValueFrom(this.http.get<any>(`/projects/${key}`)).then(res => {
            if (res?.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Project not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<any>('/projects')).then(res => {
            if (res && Array.isArray(res.projects)) {
              return res.projects.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      })
    };
  }

  createTaskTypeDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key) => {
          return lastValueFrom(this.http.get<ApiResponse<any>>(`/task-types/${key}`)).then(res => {
            if (res?.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Task type not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<{ task_types: any[] }>('/task-types')).then(res => {
            if (res && Array.isArray(res.task_types)) {
              return res.task_types.map(t => ({ id: t.id, name: t.name }));
            }
            return [];
          });
        }
      })
    };
  }

  createTaskStatusDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key) => {
          return lastValueFrom(this.http.get<ApiResponse<any>>(`/task-status/${key}`)).then(res => {
            if (res?.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Task status not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<{ data: any[] }>('/task-status')).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map(s => ({ id: s.id, name: s.name }));
            }
            return [];
          });
        }
      })
    };
  }

  createMilestoneLookupDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key) => {
          return lastValueFrom(this.http.get<ApiResponse<any>>(`/milestones/${key}`)).then(res => {
            if (res?.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Milestone not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<ApiResponse<any[]>>('/milestones')).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map(m => ({ id: m.id, name: m.name }));
            }
            return [];
          });
        }
      })
    };
  }
}
