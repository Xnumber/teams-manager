import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { CustomStore } from 'devextreme/common/data';
import { Team } from '../teams/teams-data-source.service';

@Injectable()
export class ProjectMilestonesLookupDataSourceService {
  projectId: string | null = null;
  
  constructor(private http: HttpClient) {}

  createMilestoneLookupDataSource = () => {
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<any>(`/milestones/${key}`)).then(res => {
            if (res && res.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Milestone not found');
          });
        },
        load: () => {
          console.log('Loading milestones for project_id:', this.projectId);
          return lastValueFrom(this.http.get<any>("/milestones", { params: { project_id: this.projectId ?? '' } })).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      }),
      filter: ['project_id', '=', this.projectId]
    };
  }

  createProjectLookupDataSource() {
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<any>(`/projects/${key}`)).then(res => {
            if (res && res.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Project not found');
          });
        },
        load: (loadOptions) => {
          const filter = loadOptions.searchValue ?? undefined;
          const params: Record<string, any> = {};
          if (filter) {
            params['filter'] = filter;
          }
          return lastValueFrom(this.http.get<any>(`/projects`, { params })).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      })
    };
  }

  createTeamLookupDataSource() {
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<ApiResponse<Team>>(`/teams/${key}`)).then(res => {
            if (res && res.data) {
              return { id: res.data.id, name: res.data.name };
            }
            throw new Error('Team not found');
          });
        },
        load: () => {
          return lastValueFrom(this.http.get<ApiResponse<Team>>(`/teams`)).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map((t: Team) => ({ id: t.id, name: t.name }));
            }
            return [];
          });
        }
      })
    };
  }

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
