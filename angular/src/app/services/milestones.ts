import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { CustomStore, DataSourceOptions, LoadOptions } from 'devextreme-angular/common/data';
import { Milestone } from '../types/milestone';



@Injectable({
  providedIn: 'root',
})
export class MilestonesService {
  constructor(private http: HttpClient) { }

  reprioritize(milestoneId: string, priority: number, tenantId: string): Promise<void> {
    return lastValueFrom(
      this.http.put<{ result: string; message?: string }>(`/milestone/reprioritize`, {
        milestone_id: milestoneId,
        tenant_id: tenantId, // TODO: remove tenant_id after backend support
        priority,
      })
    ).then((res) => {
      if (res?.result !== 'ok') {
        throw new Error(res?.message || 'Reprioritize failed');
      }
    });
  }



  
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
        load: (loadOptions: LoadOptions) => {
          const filter = loadOptions.searchValue
          const params: Record<string, any> = {}
          if (filter) {
            params['filter'] = filter;
          }
          // console.log('Loading milestones for project_id:', this.projectId);
          return lastValueFrom(this.http.get<any>("/milestones", { params })).then(res => {
            if (res && Array.isArray(res.data)) {
              return res.data.map((p: any) => ({ id: p.id, name: p.name }));
            }
            return [];
          });
        }
      })
    };
  }
}