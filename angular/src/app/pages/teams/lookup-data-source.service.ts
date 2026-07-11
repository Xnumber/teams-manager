import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { CustomStore } from 'devextreme/common/data';

@Injectable()
export class TeamsLookupDataSourceService {
  constructor(private http: HttpClient) {}

  createTenantLookupDataSource() {
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<any>(`/tenant/${key}`)).then(res => {
            if (res && res.tenant) {
              return { id: res.tenant.id, name: res.tenant.name };
            }
            throw new Error('Tenant not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<any>(`/tenant`)).then(res => {
            if (res && Array.isArray(res.tenants)) {
              return res.tenants.map((t: any) => ({ id: t.id, name: t.name }));
            }
            return [];
          });
        }
      })
    };
  }
}
