import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { Milestone } from '../../types/milestone';

@Injectable({
  providedIn: 'root',
})
export class MilestonesDataSourceService {
  constructor(private http: HttpClient) {}
  
  createMilestonesDataSource(): DataSource<any> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<ApiResponse<Milestone>>(`/milestones`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return data.data;
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<ApiResponse<Milestone>>(`/milestones`, values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.result || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
        return lastValueFrom(this.http.put<ApiResponse<Milestone>>(`/milestones/${key.id}`, {
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
        return lastValueFrom(this.http.delete<ApiResponse<Milestone>>(`/milestones/${key.id}`))
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
