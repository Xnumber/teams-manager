import { Component, signal } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import DataSource from 'devextreme/data/data_source';
import { DxDataGridModule } from 'devextreme-angular';
import { lastValueFrom } from 'rxjs';
import { JBTask, RawTask } from '../../tasks/typing';
import { Auth } from '../../../services/auth';

@Component({
  selector: 'app-completed-tasks',
  imports: [DxDataGridModule],
  templateUrl: './completed-tasks.html',
  styleUrl: './completed-tasks.scss',
})
export class CompletedTasks {
  completedTasks: DataSource<JBTask>;
  taskCount = signal(0);

  constructor(
    private http: HttpClient,
    private auth: Auth
  ) {
    const userId = this.auth.getUserId();
    this.completedTasks = this.createCompletedThisMonthDataSource(userId);

    this.completedTasks.store().on('loaded', (e: any) => {
      if (typeof e.totalCount === 'number') {
        this.taskCount.set(e.totalCount);
      }
    });
  }

  private createCompletedThisMonthDataSource(userId: string | null): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        const url = '/completed-this-month-tasks';
        const params: Record<string, string> = {};

        if (userId) {
          params['executor_id'] = userId;
          params['mentor_id'] = userId;
        }

        return lastValueFrom(
          this.http.get<{ count: number; result: string; tasks: RawTask[] }>(url, { params })
        ).then((data) => {
          if (data?.result === 'ok' && Array.isArray(data.tasks)) {
            return {
              data: data.tasks.map((task) => ({
                ...task,
                executor_ids: task.executors?.map((e: any) => e.user_id) || [],
                mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
              }) as JBTask),
              totalCount: data.count,
            };
          }

          return {
            data: [],
            totalCount: 0,
          };
        });
      },
    });
  }

  displayProgress = (row: JBTask): string => {
    return `${Math.round((row.progress || 0) * 100)}%`;
  }

}
