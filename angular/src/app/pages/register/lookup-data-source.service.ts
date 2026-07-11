import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { CustomStore, DataSourceOptions, LoadOptions } from 'devextreme/common/data';
import { Project } from '../../types/project';
import { User } from '../../types/user';
import { TaskType } from '../../types/task-types';



@Injectable()
export class LookupDataSourceService {
    selectedCreatorId: string | null = null;
    constructor(private http: HttpClient) { }
    // createExecutorDataSource(): DataSource<any> {
    createUserDataSource(): DataSourceOptions {
        return {
            store: new CustomStore({
                byKey: (key) => {
                    return lastValueFrom(this.http.get<ApiResponse<User>>(`/user/${key}`)).then(res => {
                        if (res && res.data) {
                            return { id: res.data.id, name: res.data.username };
                        }
                        throw new Error('User not found');
                    });
                },
                load: (loadOptions: LoadOptions) => {
                    return lastValueFrom(this.http.get<ApiResponse<User[]>>("/user")).then(res => {
                        if (res && Array.isArray(res.data)) {
                            // 預期 user 欄位有 id, username 或 name
                            return [...res.data.map(u => ({ id: u.id, name: u.username  }))];
                        }
                        return [];
                    });
                }
            })
        }
    }
}