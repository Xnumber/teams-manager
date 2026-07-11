import { Injectable } from '@angular/core';
import { DataSource } from 'devextreme/common/data';
import { HttpClient } from '@angular/common/http';
import { Team } from '../../types/team';
import { lastValueFrom } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class Data {
constructor(private http: HttpClient) {}
createTeamsDataSource(): DataSource<Team> {
    return new DataSource({
      load: () => {
        return lastValueFrom(this.http.get<{ result: string; data: Team[] }>(`/teams`))
          .then(data => {
            if (data?.result === 'ok' && Array.isArray(data.data)) {
              return data.data;
            }
            return [];
          });
      },
      // insert: (values: Record<string, any>) => {
      //   return lastValueFrom(this.http.post<{ result: string; data: Team }>(`/teams`, values))
      //     .then(res => {
      //       if (res?.result === 'ok' && res.data) {
      //         return res.data;
      //       }
      //       throw new Error(res?.result || 'Create failed');
      //     });
      // },
      // update: (key: any, values: Record<string, any>) => {
      //   return lastValueFrom(this.http.put<{ result: string; data: Team }>(`/teams/${key.id}`, {
      //     ...key,
      //     ...values
      //   }))
      //     .then(res => {
      //       if (res?.result === 'ok' && res.data) {
      //         return res.data;
      //       }
      //       throw new Error(res?.result || 'Update failed');
      //     });
      // },
      // remove: (key: any) => {
      //   return lastValueFrom(this.http.delete<{ result: string; data: Team }>(`/teams/${key.id}`))
      //     .then(res => {
      //       if (res?.result === 'ok') {
      //         return key;
      //       }
      //       throw new Error(res?.result || 'Delete failed');
      //     });
      // }
    });
  }
}
