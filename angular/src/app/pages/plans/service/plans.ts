import { Injectable } from '@angular/core';
import { lastValueFrom } from 'rxjs/internal/lastValueFrom';
import { DataSource } from 'devextreme-angular/common/data';
import { HttpClient } from '@angular/common/http';


type PlanStatus = 'draft' | 'active' | 'completed' | 'cancelled';
type Plan = {
  id: string;
  name: string;
  description: string;
  start_date: string | Date;
  end_date: string | Date;
  created_at: string | Date;
  status: PlanStatus;
}


@Injectable()
export class PlanService {
  constructor(private http: HttpClient) {}
  createPlansDataSource(): DataSource<Plan> {
      return new DataSource({
        load: () => {
          return lastValueFrom(this.http.get<{ result: string; data: Plan[] }>(`/plans`))
            .then(data => {
              if (data?.result === 'ok' && Array.isArray(data.data)) {
                return data.data;
              }
              return [];
            });
        },
        insert: (values: Record<string, any>) => {
          return lastValueFrom(this.http.post<{ result: string; data: Plan }>(`/plans`, values))
            .then(res => {
              if (res?.result === 'ok' && res.data) {
                return res.data;
              }
              throw new Error(res?.result || 'Create failed');
            });
        },
        update: (key: any, values: Record<string, any>) => {
          return lastValueFrom(this.http.put<{ result: string; data: Plan }>(`/plans/${key.id}`, {
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
          return lastValueFrom(this.http.delete<{ result: string; data: Plan }>(`/plans/${key.id}`))
            .then(res => {
              if (res?.result === 'ok') {
                return key;
              }
              throw new Error(res?.result || 'Delete failed');
            });
        }
      });
    }
    

    getPlanHistory(planId: string): Promise<any[]> {
      return lastValueFrom(this.http.get<{ result: string; data: any[] }>(`/plan-history/${planId}`))
        .then(res => {
          if (res?.result === 'ok' && Array.isArray(res.data)) {
            return res.data;
          }
          throw new Error(res?.result || 'Failed to fetch plan history');
        });
    }
}
