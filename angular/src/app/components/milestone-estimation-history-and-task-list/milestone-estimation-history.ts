import { HttpClient } from '@angular/common/http';
import { Component, inject, input, Input } from '@angular/core';
import { combineLatest, filter, switchMap, map, BehaviorSubject } from 'rxjs';
import { DxButtonModule, DxChartModule, DxSelectBoxModule } from 'devextreme-angular';
import { ValueChangedEvent } from 'devextreme/ui/select_box';
import { CommonModule } from '@angular/common';
import { AnalyticsLookupDataSourceService } from '../../pages/analytics/lookup-data-source.service';
import { MilestoneEstimationHistoryData } from '../../pages/analytics/type';

@Component({
  selector: 'app-milestone-estimation-history-and-task-list',
  imports: [
    DxButtonModule, 
    DxSelectBoxModule, 
    DxChartModule,
    CommonModule
  ],
  templateUrl: './milestone-estimation-history.html',
  styleUrl: './milestone-estimation-history.scss',
})
export class MilestoneEstimationHistoryAndTaskList {
  projectId = input<string>('');
  projectName = input<string>('');
  http = inject(HttpClient);
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
  milestoneConvergenceData = combineLatest([this.milestoneId$, this.reload$])
  .pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get('/milestone-acceptance-time-convergence', { params: { milestone_id: id! } }).pipe(
        map((res: any) => res.data?.histories?.map((item: MilestoneEstimationHistoryData) => ({
          // x: item.estimation_date ? item.estimation_date.replace(/T.*$/, '') : item.estimation_date,
          x: new Date(item.estimation_date),
          complete_date: new Date(item.complete_date),
          optimistic_estimated_complete_date: new Date(item.optimistic_estimated_complete_date),
          pessimistic_estimated_complete_date: new Date(item.pessimistic_estimated_complete_date),
        })) || [])
      )
    )
  );
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
}