
import DataSource from 'devextreme/data/data_source';
import { lastValueFrom } from 'rxjs';

import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';
import { firstValueFrom } from 'rxjs';



export interface SyncPrsParams {
  repo_id: string;
  github_token: string;
}

@Injectable({
  providedIn: 'root',
})
export class Data {
  constructor(private http: HttpClient) { }

  syncPrs(params: SyncPrsParams, githubToken?: string) {
    const headers: any = githubToken
      ? { Authorization: `Bearer ${githubToken}` }
      : {};
    return firstValueFrom(
      this.http.post<any>(
        '/sync-pull-requests',
        params,
        { headers }
      )
    );
  }

  getRepositoriesDataSource() {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<any>('/repositories')).then(res => {
          if (res && res.result === 'ok' && Array.isArray(res.data)) {
            return res.data.map((r: any) => ({
              id: r.repo_id,
              name: `${r.owner}/${r.repo_name}`
            }));
          }
          return [];
        });
      },
      byKey: (key: number) => {
        return lastValueFrom(this.http.get<any>(`/repositories/${key}`)).then(res => {
          if (res && res.result === 'ok' && res.data) {
            return { repo_id: res.data.repo_id, repo_name: res.data.repo_name };
          }
          throw new Error('Repository not found');
        });
      }
    });
  }

  createPullRequestDataSource(repoId: string | number) {
    return new DataSource<any>({
      load: () => {
        if (!repoId) return [];
        return lastValueFrom(this.http.get<any>(`/pull-requests?repo_id=${repoId}`)).then(res => {
          if (res && res.result === 'ok' && Array.isArray(res.data)) {
            return res.data;
          }
          return [];
        });
      }
    });
  }



  
  syncPrsAndUsers() {
    return firstValueFrom(this.http.post<any>(
      '/sync-pull-requests-and-users',
      {},
    ));
  }
}
