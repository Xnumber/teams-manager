import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';

export interface Role {
  id: string;
  name: string;
  description?: string;
}

@Injectable({
  providedIn: 'root',
})
export class RolesDataSourceService {
  constructor(private http: HttpClient) {}

  createRolesDataSource(): DataSource<Role> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<{ count: number; result: string; roles: Role[] }>(`/roles`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.roles)) {
              return data.roles;
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<{ result: string; data: Role }>(`/roles`, values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.result || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
        return lastValueFrom(this.http.put<{ result: string; data: Role }>(`/roles/${key.id}`, {
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
        return lastValueFrom(this.http.delete<{ result: string; role: Role }>(`/roles/${key.id}`))
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
