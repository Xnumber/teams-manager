import { Component, inject, signal } from '@angular/core';
import { DxChartModule, DxDataGridModule  } from 'devextreme-angular';
import { ProjectMetricsHistoriesDataSourceService } from './projects-data-source.service';
import { ProjectMetricsHistory } from './type';
import { aggregateCountByDate } from './utils';
import { of } from 'rxjs';
import { HttpClient } from '@angular/common/http';
import { switchMap } from 'rxjs/operators';

import { formatDate } from 'devextreme/localization';

@Component({
  selector: 'app-project-metrics-histories',
  imports: [
    DxChartModule,
    DxDataGridModule
  ],
  templateUrl: './project-metrics-histories.html',
  styleUrl: './project-metrics-histories.scss',
})
export class ProjectMetricsHistories {
  projectMetricsHistoriesDataSourceService = inject(ProjectMetricsHistoriesDataSourceService);
  milestonesDataSource: { date: string; milestone_count: number; milestone_added_count: number; milestone_completed_count: number }[] = [];
  tasksDataSource: { date: string; task_count: number; task_added_count: number; task_completed_count: number }[] = [];
  http = inject(HttpClient);

  totalColor = '#3C1642';
  addedColor = '#086375';
  completedColor = '#1DD3B0';

  milestones = signal(0);
  milestoneAdded = signal(0);
  milestoneCompleted = signal(0);
  tasks = signal(0);
  taskAdded = signal(0);
  taskCompleted = signal(0);

  constructor() {
    const dataSource = this.projectMetricsHistoriesDataSourceService.createProjectMetricsHistoriesDataSource();

    of(this.http.put<ProjectMetricsHistory[]>('/project_metrics_history/update_task_count_history', {}).toPromise())
      .pipe(
        switchMap(() => {
          dataSource.load().then((data: ProjectMetricsHistory[]) => {

            this.milestonesDataSource = aggregateCountByDate(data, [
              'milestone_count',
              'milestone_added_count',
              'milestone_completed_count'
            ]);

            const latestData = this.milestonesDataSource[this.milestonesDataSource.length - 1];
            if (latestData) {
              this.milestones.set(latestData.milestone_count);
              this.milestoneAdded.set(latestData.milestone_added_count);
              this.milestoneCompleted.set(latestData.milestone_completed_count);
            }
            this.tasksDataSource = aggregateCountByDate(data, [
              'task_count',
              'task_added_count',
              'task_completed_count'
            ]);
            const latestTaskData = this.tasksDataSource[this.tasksDataSource.length - 1];
            if (latestTaskData) {
              this.tasks.set(latestTaskData.task_count);
              this.taskAdded.set(latestTaskData.task_added_count);
              this.taskCompleted.set(latestTaskData.task_completed_count);
            }
            console.log('tasksDataSource', this.tasksDataSource);
            console.log('milestonesDataSource', this.milestonesDataSource);
          });
          return of(null);
        })
      ).subscribe();
  }


  // https://js.devexpress.com/Angular/Documentation/24_2/ApiReference/UI_Components/dxChart/Configuration/tooltip/#customizeTooltip
  customTooltip(pointInfo: any): { text: string } {
    
    
    const date = new Date(pointInfo.argumentText);
    const formattedDate = formatDate(date, 'yyyy/MM/dd');
    return {
      text: `${formattedDate} - ${pointInfo.valueText} 項`
    };
  }
}