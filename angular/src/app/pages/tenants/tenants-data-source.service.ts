import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { Tenant } from '../../types/tenant';

@Injectable({
  providedIn: 'root',
})
export class TenantsDataSourceService {
  constructor(private http: HttpClient) {}

  createTenantsDataSource(): DataSource<Tenant> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<{ count: number; result: string; tenants: Tenant[] }>(`/tenant`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.tenants)) {
              return data.tenants;
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<{ result: string; tenant: Tenant }>(`/tenant`, values))
          .then(res => {
            if (res?.result === 'ok' && res.tenant) {
              return res.tenant;
            }
            throw new Error(res?.result || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
        return lastValueFrom(this.http.put<{ result: string; tenant: Tenant }>(`/tenant/${key.id}`, {
          ...key,
          ...values
        }))
          .then(res => {
            if (res?.result === 'ok' && res.tenant) {
              return res.tenant;
            }
            throw new Error(res?.result || 'Update failed');
          });
      },
      remove: (key: any) => {
        return lastValueFrom(this.http.delete<{ result: string; tenant: Tenant }>(`/tenant/${key.id}`))
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
