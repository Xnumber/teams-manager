import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { Project } from '../../types/project';

@Injectable({
  providedIn: 'root',
})
export class ProjectsDataSourceService {
  constructor(private http: HttpClient) {}

  reprioritize(projectId: string, priority: number, tenantId: string): Promise<void> {
    return lastValueFrom(
      this.http.put<{ result: string; message?: string }>(`/project/reprioritize`, {
        project_id: projectId,
        priority,
        tenant_id: tenantId, // TODO: remove tenant_id after backend support
      })
    ).then((res) => {
      if (res?.result !== 'ok') {
        throw new Error(res?.message || 'Reprioritize failed');
      }
    });
  }

  reprioritizeMilestone(milestoneId: string, priority: number, tenantId: string): Promise<void> {
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

  createProjectsDataSource(): DataSource<Project> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<{ count: number; result: string; data: Project[] }>(`/projects`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return data.data || [];
            }
            return [];
          });
      },
      insert: (values: Record<string, any>) => {
        return lastValueFrom(this.http.post<{ result: string; data: Project }>(`/projects`, values))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return res.data;
            }
            throw new Error(res?.result || 'Create failed');
          });
      },
      update: (key: any, values: Record<string, any>) => {
        return lastValueFrom(this.http.put<{ result: string; data: Project }>(`/projects/${key.id}`, {
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
        return lastValueFrom(this.http.delete<{ result: string; project: Project }>(`/projects/${key.id}`))
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
