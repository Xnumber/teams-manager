import { Injectable } from '@angular/core';
import CustomStore from 'devextreme/data/custom_store';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { ApiResponse, User } from './typing';

@Injectable({ providedIn: 'root' })
export class LookupDataSourceService {
  constructor(private http: HttpClient) {}

  createTenantDataSource() {
    return {
      store: new CustomStore({
        key: 'id',
        byKey: (key) => {
          return lastValueFrom(this.http.get<ApiResponse<any>>(`/tenant/${key}`)).then(res => {
            if (res && res.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Tenant not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<{ tenants: any[] }>("/tenant")).then(res => {
            if (res && Array.isArray(res.tenants)) {
              return res.tenants.map(t => ({ id: t.id, name: t.name }));
            }
            return [];
          });
        }
      })
    };
  }

  createTeamDataSource() {
    return {
      store: new CustomStore({
        key: 'id',
        byKey: (key) => {
          return lastValueFrom(this.http.get<ApiResponse<any>>(`/teams/${key}`)).then(res => {
            if (res && res.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Team not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<{ data: any[] }>("/teams")).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map(t => ({ id: t.id, name: t.name }));
            }
            return [];
          });
        }
      })
    };
  }
}
