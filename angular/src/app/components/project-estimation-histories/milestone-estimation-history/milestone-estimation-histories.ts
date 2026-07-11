import { HttpClient } from '@angular/common/http';

import { Component, inject, input, Input } from '@angular/core';
import { combineLatest, filter, switchMap, map, BehaviorSubject } from 'rxjs';
import { DxButtonModule, DxChartModule, DxSelectBoxModule, DxDataGridModule } from 'devextreme-angular';
import { ValueChangedEvent } from 'devextreme/ui/select_box';
import { CommonModule } from '@angular/common';
import { AnalyticsLookupDataSourceService } from '../../../pages/analytics/lookup-data-source.service';
import { MilestoneEstimationHistoryData } from '../../../pages/analytics/type';
import { Column } from 'devextreme/ui/data_grid';

@Component({
  selector: 'app-milestone-estimation-history',
  imports: [
    DxButtonModule,
    DxSelectBoxModule,
    DxChartModule,
    CommonModule,
    DxDataGridModule
],
  templateUrl: './milestone-estimation-histories.html',
  styleUrl: './milestone-estimation-histories.scss',
})
export class MilestoneEstimationHistory {
  http = inject(HttpClient);
  selectedeChartTypes = input<("left" | "delta" | "convergence" | "list")[]>([
    'convergence',
    'left',
    'delta',
    'list'
  ]);
  analyticsLookupDataSourceService = inject(AnalyticsLookupDataSourceService);
  milestoneName = input<string>('');
  private milestoneId$ = new BehaviorSubject<string | null>(null);
  private reload$ = new BehaviorSubject<void>(undefined);
  private _milestoneId: string | null = null;

  showUpdateButton = input<boolean>(false);

  @Input()
  set milestoneId(value: string | null) {
    const id = (value ?? '').toString().trim() || null;
    this._milestoneId = id;
    this.milestoneId$.next(id);
  }
  get milestoneId(): string | null {
    return this._milestoneId;
  }

  // milestoneLookupDataSource = this.analyticsLookupDataSourceService.createMilestoneLookupDataSource();
  leftTasksRangeStart = 0;
  leftTasksRangeEnd = 0;
  cumulativeWorkingDayDeltaRangeStart = 0;
  cumulativeWorkingDayDeltaRangeEnd = 0;

  milestoneConvergenceData = combineLatest([this.milestoneId$, this.reload$]).pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get('/milestone-acceptance-time-convergence', { params: { milestone_id: id! } }).pipe(
        map((res: any) => {
          const histories = res.data?.histories?.map((item: MilestoneEstimationHistoryData) => ({
            x: new Date(item.estimation_date),
            complete_date: new Date(item.complete_date),
            optimistic_estimated_complete_date: new Date(item.optimistic_estimated_complete_date),
            pessimistic_estimated_complete_date: new Date(item.pessimistic_estimated_complete_date),
            left_tasks: item.left_tasks,
          })) || [];

          const completeDates = histories
            .map((item: { complete_date: Date }) => item.complete_date)
            .filter((date: Date) => !Number.isNaN(date.getTime()));

          if (completeDates.length > 0) {
            const minCompleteDate = new Date(Math.min(...completeDates.map((date: Date) => date.getTime())));
            const maxCompleteDate = new Date(Math.max(...completeDates.map((date: Date) => date.getTime())));

            this.convergenceStartDate = new Date(minCompleteDate);
            this.convergenceStartDate.setMonth(this.convergenceStartDate.getMonth() - 3);

            this.convergenceEndDate = new Date(maxCompleteDate);
            this.convergenceEndDate.setMonth(this.convergenceEndDate.getMonth() + 3);
          }

          // Calculate leftTasksRangeStart and leftTasksRangeEnd from the last data point
          if (histories.length > 0) {
            const lastLeftTasks = histories[histories.length - 1].left_tasks;
            this.leftTasksRangeStart = Math.max(0, lastLeftTasks - 15);
            this.leftTasksRangeEnd = lastLeftTasks + 15;
          }

          return histories;
        })
      )
    )
  );

  cumulativeWorkingDayDeltaData = combineLatest([this.milestoneId$, this.reload$]).pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get('/milestones/get-cumulative-working-day-delta', { params: { milestone_id: id! } }).pipe(
        map((res: any) => {
          const data = res.data?.map((item: {
            estimation_date: string;
            delta: number;
          }) => ({
            x: new Date(item.estimation_date),
            delta: item.delta,
          })) || [];

          // Calculate cumulativeWorkingDayDeltaRangeStart and cumulativeWorkingDayDeltaRangeEnd from the last data point
          if (data.length > 0) {
            const lastDelta = data[data.length - 1].delta;
            this.cumulativeWorkingDayDeltaRangeStart = lastDelta - 15;
            this.cumulativeWorkingDayDeltaRangeEnd = lastDelta + 15;
          }

          return data;
        })
      )
    )
  );
  
  
  tasksDataSource$ = combineLatest([this.milestoneId$, this.reload$])
  .pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get('/tasks-filter', { params: { 
        milestone_id: id!,
        completed: false
      } }).pipe(
        map((res: any) => res.data || [])
      )
    )
  )

  milestoneTasksColumns: Column[] = [
    {
      dataField: 'name',
      caption: '待辦'
    },
    {
      dataField: 'estimated_workdays',
      caption: '預估工作天'
    }
  ];


  mileStoneChanged(e: ValueChangedEvent) {
    this.milestoneId = e.value ?? null;
    console.log('Selected Milestone ID:', this.milestoneId);
  }







  
  updateMileStoneInfo() {
    if (!this.milestoneId) {
      return;
    }

    this.http.put(`/milestone-acceptance-time-convergence2?milestone_id=${this.milestoneId}`, {}).subscribe(() => {
      this.reload$.next();
    });

    // 在這裡根據選擇的工作大項ID更新圖表數據
    // 例如，您可以調用一個服務方法來獲取該工作大項相關的數據，然後更新 this.data 或其他相關屬性
  }

  convergenceStartDate = new Date();
  convergenceEndDate = new Date();
}