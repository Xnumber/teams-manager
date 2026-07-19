import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import CustomStore from 'devextreme/data/custom_store';
import { DataSourceOptions, LoadOptions } from 'devextreme-angular/common/data';
import { User } from '../types/user';

@Injectable({
  providedIn: 'root',
})
export class UserService {
  constructor(private http: HttpClient) {}

  createUserDataSource(): DataSourceOptions {
    return {
      store: new CustomStore({
        byKey: (key: string) => {
          return lastValueFrom(this.http.get<ApiResponse<User>>(`/user/${key}`)).then((res) => {
            if (res?.data) {
              return { id: res.data.id, name: res.data.username };
            }

            throw new Error('User not found');
          });
        },
        load: (_loadOptions: LoadOptions) => {
          return lastValueFrom(this.http.get<ApiResponse<User[]>>('/user')).then((res) => {
            if (res?.data && Array.isArray(res.data)) {
              return res.data.map((user) => ({ id: user.id, name: user.username }));
            }

            return [];
          });
        },
      }),
    };
  }
}