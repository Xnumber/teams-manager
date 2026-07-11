import { Injectable } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import CustomStore from 'devextreme/data/custom_store';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { User, ApiResponse } from './typing';

@Injectable({
  providedIn: 'root',
})
export class UsersDataSourceService {
  constructor(private http: HttpClient) {}

  createUsersDataSource(): DataSource<User> {
    return new DataSource({
      store: new CustomStore({
        key: 'id',
        load: () => {
          return lastValueFrom(this.http.get<ApiResponse<User[]>>('/user'))
            .then(res => res.data || []);
        },
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<ApiResponse<User>>(`/user/${key}`))
            .then(res => res.data);
        },
        insert: (values: Partial<User>) => {
          return lastValueFrom(this.http.post<ApiResponse<User>>('/user', values))
            .then(res => res.data);
        },
        update: (key: string, values: Partial<User>) => {
          return lastValueFrom(this.http.put<ApiResponse<User>>(`/user/${key}`, values))
            .then(res => res.data);
        },
        remove: (key: string) => {
          return lastValueFrom(this.http.delete<ApiResponse<User>>(`/user/${key}`)).then(() => {});
        }
      })
    });
  }
}
