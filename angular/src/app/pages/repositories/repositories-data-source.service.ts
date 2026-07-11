import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';

export interface Repository {
  id: string;
  repo_id: number;
  repo_name: string;
  created_at?: string;
}

@Injectable({
  providedIn: 'root',
})
export class RepositoriesDataSourceService {
  constructor(private http: HttpClient) {}

  createRepositoriesDataSource(): DataSource<Repository> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<{ result: string; data: Repository[] }>(`/repositories`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return data.data;
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<{ result: string; data: Repository }>(`/repositories`, values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.result || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
        // 主鍵複合: id:uuid, repo_id:number
        const pk = `${key.id}:${key.repo_id}`;
        return lastValueFrom(this.http.put<{ result: string; data: Repository }>(`/repositories/${pk}`, {
          ...key,
          ...values
        }))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.result || 'Update failed');
          });
      },
      remove: (key: any) => {
        const pk = `${key.id}:${key.repo_id}`;
        return lastValueFrom(this.http.delete<{ result: string; data: Repository }>(`/repositories/${pk}`))
          .then(res => {
            if (res?.result === 'ok') {
              return key;
            }
            throw new Error(res?.result || 'Delete failed');
          });
      }
    });
  }
}
