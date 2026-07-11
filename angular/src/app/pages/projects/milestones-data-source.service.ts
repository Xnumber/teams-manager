import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import DataSource from 'devextreme/data/data_source';
import { lastValueFrom } from 'rxjs';
import { Milestone } from '../../types/milestone';

@Injectable({
  providedIn: 'root',
})
export class ProjectMilestonesDataSourceService {
  constructor(private http: HttpClient) {}

  createMilestonesDataSource(projectId: string): DataSource<Milestone> {
    return new DataSource<Milestone>({
      load: () => {
        return lastValueFrom(
          this.http.get<ApiResponse<Milestone>>('/milestones', {
            params: { project_id: projectId }
          })
        ).then(data => {
          if (data?.result === 'ok' && Array.isArray(data.data)) {
            return data.data;
          }
          return [];
        });
      }
    });
  }
}