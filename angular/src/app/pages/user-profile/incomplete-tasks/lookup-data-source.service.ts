import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { CustomStore, DataSourceOptions, LoadOptions } from 'devextreme/common/data';
import { User } from '../../../types/user';
import { TaskType } from '../../task-types/typing';
import { Project } from '../../../types/project';




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


    createTaskTypeDataSource(): DataSourceOptions {
        return {
            store: new CustomStore({
                byKey: (key) => {
                    return lastValueFrom(this.http.get<ApiResponse<TaskType>>(`/task-types/${key}`)).then(res => {
                        if (res && res.data) {
                            return { id: res.data.id, name: res.data.name };
                        }
                        throw new Error('Task type not found');
                    });
                },
                load: () => {
                    return lastValueFrom(this.http.get<{ task_types: any[] }>("/task-types")).then(res => {
                        if (res && Array.isArray(res.task_types)) {
                            // 預期 taskType 欄位有 id, name
                            return [ ...res.task_types.map(t => ({ id: t.id, name: t.name }))];
                        }
                        return [];
                    });
                }
            })
        }
    }

    createProjectDataSource(): DataSourceOptions {
        return {
            store: new CustomStore({
                byKey: (key) => {
                    return lastValueFrom(this.http.get<ApiResponse<Project>>(`/projects/${key}`)).then(res => {
                        if (res) {
                            return { id: res.data?.id, name: res.data?.name };
                        }
                        throw new Error('Project not found');
                    });
                },
                load: () => {
                    return lastValueFrom(this.http.get<{ projects: any[] }>("/projects")).then(res => {
                        if (res && Array.isArray(res.projects)) {
                            // 預期 project 欄位有 id, name
                            return [...res.projects.map(p => ({ id: p.id, name: p.name }))];
                        }
                        return [];
                    });
                }
            })
        }
    }

    createTaskStatusDataSource(): DataSourceOptions {
        return {
            store: new CustomStore({
                byKey: (key) => {
                    return lastValueFrom(this.http.get<ApiResponse<Project>>(`/task-status/${key}`)).then(res => {
                        if (res) {
                            return { id: res.data?.id, name: res.data?.name };
                        }
                        throw new Error('Project not found');
                    });
                },
                load: () => {
                    return lastValueFrom(this.http.get<{ data: any[] }>("/task-status")).then(res => {
                        if (res && Array.isArray(res.data)) {
                            // 預期 project 欄位有 id, name
                            return [...res.data.map(p => ({ id: p.id, name: p.name }))];
                        }
                        return [];
                    });
                }
            })
        }
    }
}