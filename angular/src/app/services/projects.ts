import { Injectable } from '@angular/core';
import { HttpClient, HttpErrorResponse } from '@angular/common/http';
import { lastValueFrom, map } from 'rxjs';
import { Project, UserProjectSettings } from '../types/project';
import { CustomStore, DataSource, DataSourceOptions, LoadOptions } from 'devextreme-angular/common/data';



@Injectable({
  providedIn: 'root',
})
export class ProjectsService {
  constructor(private http: HttpClient) { }

  getParticipantsInfo(projectId: string): Promise<{
    count: number;
    avg_excecutor_time_ratio: number;
    sum_estimated_workdays: number;
  }> {
    return lastValueFrom(
      this.http.get<{ data: { count: number; avg_excecutor_time_ratio: number; sum_estimated_workdays: number } }>(`/projects/${projectId}/participants-info`)
        .pipe(map(res => res.data || { count: 0, avg_excecutor_time_ratio: 0, sum_estimated_workdays: 0 }))
    );
  }

  createProfileProjectSettingsDataSource(): DataSource<UserProjectSettings> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<{ count: number; result: string; data: UserProjectSettings[] }>(`/user-project-settings`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return data.data || [];
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<{ result: string; data: UserProjectSettings, message: string }>(`/user-project-settings`, values))
          .catch((error: HttpErrorResponse) => {
            throw error instanceof Error ? error : new Error(error.error?.message || 'Create failed');
          })
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.message || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
          return lastValueFrom(this.http.put<{ result: string; data: UserProjectSettings, message: string }>(`/user-project-settings/${key.id}`, {
            ...key,
            ...values
          }))
          .catch((error: HttpErrorResponse) => {
            throw error instanceof Error ? error : new Error(error.error?.message || 'Create failed');
          })
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.message || 'Update failed');
          });
      },
      // remove: (key: any) => {
      //     return lastValueFrom(this.http.delete<{ result: string; data: UserProjectSettings }>(`/project-settings/${key.id}`))
      //     .then(res => {
      //         if (res?.result === 'ok') {
      //         return key;
      //         }
      //         throw new Error(res?.message || 'Delete failed');
      //     });
      // }
    });
  }

  createProjectLookupDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key: any) => {
          return lastValueFrom(this.http.get<{ result: string; data: Project, message?: string }>(`/projects/${key.id}`))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Project not found');
            });
        },
        load: () => {
          return lastValueFrom(this.http.get<{ count: number; result: string; data: Project[] }>(`/projects`))
            .then(data => {
              if (data?.result === 'ok' && Array.isArray(data.data)) {
                return data.data || [];
              }
              return [];
            });
        },
        insert: (values: Record<string, any>) => {
          return lastValueFrom(this.http.post<{ result: string; data: Project, message?: string }>(`/projects`, values))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Create failed');
            })
            .catch((error: unknown) => {
              console.error('Create project failed:', error);
              throw error instanceof Error ? error : new Error('Create failed');
            });
        },
        update: (key: any, values: Record<string, any>) => {
          return lastValueFrom(this.http.put<{ result: string; data: Project, message?: string }>(`/projects/${key.id}`, {
            ...key,
            ...values
          }))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Update failed');
            });
        },
        remove: (key: any) => {
          return lastValueFrom(this.http.delete<{ result: string; project: Project }>(`/projects/${key.id}`))
            .then(res => {
              if (res?.result === 'ok') {
                return key;
              }
              throw new Error(res?.result || 'Delete failed');
            });
        }
      })
    }
  }

  createProjectLookupOnlyDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key: any) => {
          return lastValueFrom(this.http.get<{ result: string; data: Project, message?: string }>(`/projects/${key.id}`))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Project not found');
            });
        },
        load: (loadOptions: LoadOptions) => {
          const filter = loadOptions.searchValue
          const params: Record<string, any> = {}
          if (filter) {
            params['filter'] = filter;
          }
          return lastValueFrom(this.http.get<{ count: number; result: string; data: Project[] }>(`/projects`, { params }))
            .then(data => {
              if (data?.result === 'ok' && Array.isArray(data.data)) {
                return data.data || [];
              }
              return [];
            });
        }
      })
    }
  }

  createUserProjectsLookupDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key: any) => {
          return lastValueFrom(this.http.get<{ result: string; data: Project, message?: string }>(`/projects/${key.id}`))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Project not found');
            });
        },
        load: () => {
          return lastValueFrom(this.http.get<{ count: number; result: string; data: Project[] }>(`/user-projects`))
            .then(data => {
              if (data?.result === 'ok' && Array.isArray(data.data)) {
                console.log('User projects loaded:', data.data);
                return data.data || [];
              }
              return [];
            });
        },
        insert: (values: Record<string, any>) => {
          return lastValueFrom(this.http.post<{ result: string; data: Project, message?: string }>(`/projects`, values))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Create failed');
            })
            .catch((error: unknown) => {
              console.error('Create project failed:', error);
              throw error instanceof Error ? error : new Error('Create failed');
            });
        },
        update: (key: any, values: Record<string, any>) => {
          return lastValueFrom(this.http.put<{ result: string; data: Project, message?: string }>(`/projects/${key.id}`, {
            ...key,
            ...values
          }))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.message || 'Update failed');
            });
        },
        remove: (key: any) => {
          return lastValueFrom(this.http.delete<{ result: string; project: Project }>(`/projects/${key.id}`))
            .then(res => {
              if (res?.result === 'ok') {
                return key;
              }
              throw new Error(res?.result || 'Delete failed');
            });
        }
      })
    }
  }
}