import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { ProjectMetricsHistory } from './type';






@Injectable({
  providedIn: 'root',
})
export class ProjectMetricsHistoriesDataSourceService {
  constructor(private http: HttpClient) {}

  

  createProjectMetricsHistoriesDataSource(): DataSource {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<ApiResponse<ProjectMetricsHistory>>(`/project_metrics_history`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return data.data || [];
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<{ result: string; data: any }>(`/projects`, values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.result || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
        return lastValueFrom(this.http.put<{ result: string; data: any }>(`/projects/${key.id}`, {
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
        return lastValueFrom(this.http.delete<{ result: string; data: any }>(`/projects/${key.id}`))
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