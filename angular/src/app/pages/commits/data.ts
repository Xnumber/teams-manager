import DataSource from 'devextreme/data/data_source';
import { lastValueFrom, firstValueFrom } from 'rxjs';
import { HttpClient } from '@angular/common/http';
import { Injectable } from '@angular/core';

export interface SyncCommitsParams {
    repo_id: string | number;
    github_token: string;
    since?: string;
    until?: string;
    sha?: string;
}

@Injectable({
    providedIn: 'root',
})
export class Data {
    constructor(private http: HttpClient) { }

    syncCommits(params: SyncCommitsParams) {
        return firstValueFrom(
            this.http.post<any>('/sync-commits', params)
        );
    }

    getRepositoriesDataSource() {
        return new DataSource({
            load: () => {
                return lastValueFrom(this.http.get<any>('/repositories')).then(res => {
                    if (res && res.result === 'ok' && Array.isArray(res.data)) {
                        return res.data.map((r: any) => ({
                            id: r.repo_id,
                            name: `${r.owner}/${r.repo_name}`,
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
            },
        });
    }

    createCommitsDataSource(repoId: string | number) {
        return new DataSource<any>({
            load: () => {
                if (!repoId) return [];
                return lastValueFrom(this.http.get<any>(`/commits?repo_id=${repoId}`)).then(res => {
                    if (res && res.result === 'ok' && Array.isArray(res.data)) {
                        return res.data;
                    }
                    return [];
                });
            },
        });
    }


    syncCommitsAndUsers() {
        return firstValueFrom(this.http.post<any>(
            '/sync-commits-and-users',
            {},
        ));
    }
}
