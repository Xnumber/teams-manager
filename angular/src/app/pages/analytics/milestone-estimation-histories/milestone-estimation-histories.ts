import { HttpClient } from '@angular/common/http';
import { Component, inject } from '@angular/core';
import { combineLatest, filter, switchMap, map, BehaviorSubject } from 'rxjs';
import { MilestoneEstimationHistoryData } from '../type';
import { DxButtonModule, DxChartModule, DxSelectBoxModule, DxTextBoxModule } from 'devextreme-angular';
import { AnalyticsLookupDataSourceService } from '../lookup-data-source.service';
import { ValueChangedEvent } from 'devextreme/ui/select_box';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-milestone-estimation-histories',
  imports: [
    DxButtonModule, 
    DxSelectBoxModule, 
    DxTextBoxModule, 
    DxChartModule,
    CommonModule
  ],
  templateUrl: './milestone-estimation-histories.html',
  styleUrl: './milestone-estimation-histories.scss',
})
export class MilestoneEstimationHistories {
  http = inject(HttpClient);
  analyticsLookupDataSourceService = inject(AnalyticsLookupDataSourceService);

  private milestoneId$ = new BehaviorSubject<string | null>(null);
  private reload$ = new BehaviorSubject<void>(undefined);
  
  milestoneId = null;
  
  milestoneLookupDataSource = this.analyticsLookupDataSourceService.createMilestoneLookupDataSource();
  
  milestoneConvergenceData = combineLatest([this.milestoneId$, this.reload$]).pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get('/milestone-acceptance-time-convergence2', { params: { milestone_id: id! } }).pipe(
        map((res: any) => res.data?.histories?.map((item: MilestoneEstimationHistoryData) => ({
          x: item.estimation_date ? item.estimation_date.replace(/T.*$/, '') : item.estimation_date,
          complete_date: new Date(item.complete_date),
          optimistic_estimated_complete_date: new Date(item.optimistic_estimated_complete_date),
          pessimistic_estimated_complete_date: new Date(item.pessimistic_estimated_complete_date),
        })) || [])
      )
    )
  );

  mileStoneChanged(e: ValueChangedEvent) {
    const milestoneId = e.value;
    this.milestoneId$.next(milestoneId);
    console.log('Selected Milestone ID:', milestoneId);
  }

  updateMileStoneInfo() {
    this.http.put(`/milestone-acceptance-time-convergence?milestone_id=${this.milestoneId}`, {}).subscribe(() => {
      this.reload$.next();
    });

    // 在這裡根據選擇的工作大項ID更新圖表數據
    // 例如，您可以調用一個服務方法來獲取該工作大項相關的數據，然後更新 this.data 或其他相關屬性
  }
}