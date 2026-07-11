import { HttpClient } from '@angular/common/http';
import { Component, effect, inject, input, signal } from '@angular/core';
import { combineLatest, filter, switchMap, map, BehaviorSubject, from, concatMap, toArray, tap } from 'rxjs';
import { MilestoneEstimationHistoryData } from '../../pages/analytics/type';
import { DxButtonModule, DxChartModule, DxSelectBoxModule, DxDataGridModule, DxLoadPanelModule, DxTagBoxModule, DxRadioGroupModule } from 'devextreme-angular';
import { MilestonesLookupDataSourceService } from '../../pages/milestones/lookup-data-source.service';
import { ValueChangedEvent } from 'devextreme/ui/select_box';
import { ValueChangedEvent as RadioValueChangedEvent } from 'devextreme/ui/radio_group';
import { CommonModule } from '@angular/common';
import { MilestoneEstimationHistory } from './milestone-estimation-history/milestone-estimation-histories';
import { ProjectsService } from '../../services/projects';
import { ProjectOverviewResponse } from './type';

@Component({
  selector: 'app-project-estimation-histories',
  imports: [
    DxButtonModule,
    DxSelectBoxModule,
    DxChartModule,
    CommonModule,
    MilestoneEstimationHistory,
    DxDataGridModule,
    DxLoadPanelModule,
    DxTagBoxModule,
    DxRadioGroupModule
],
  templateUrl: './project-estimation-histories.html',
  styleUrl: './project-estimation-histories.scss',
})
export class ProjectEstimationHistories {
  projectService = inject(ProjectsService);
  closestDemoMilestone = signal('未知工作大項');
  closestDemoMilestoneDate = signal('');
  // latestMilestoneName = signal('未知工作大項');
  inputProjectId = input<string | null>(null);
  inputProjectName = input<string | null>(null);
  inputShowMilestoneCharts = input<boolean>(true);
  selectedProjectChartTypes = input<("overview" | "left" | "delta" | "convergence" | "list")[]>(['convergence', 'delta', 'left', 'list']);
  http = inject(HttpClient);

  milestonesLookupDataSourceService = inject(MilestonesLookupDataSourceService);

  milestoneChartTypes = [
    { id: 'convergence', name: '預估下次發布日' },
    { id: 'left', name: '剩餘工作大項' },
    { id: 'delta', name: '累積超前/落後工作日' },
    { id: 'list', name: '工作列表' },
  ];

  selectedMilestoneChartTypes = signal<("left" | "delta" | "convergence" | "list")[]>(['convergence', 'delta', 'left', 'list']);
  
  private projectId$ = new BehaviorSubject<string | null>(null);
  private reload$ = new BehaviorSubject<void>(undefined);
  showMilestoneCharts = signal(false);


  chosenDisplayType = signal('incompleted');

  radioOptions = [
    { id: 'incompleted', name: '未完成' },
    // { id: 'archived', name: '已封存' },
    { id: 'completed', name: '已完成' },
    { id: 'all', name: '全部' },
  ]
  

  chosenDisplayTypeChanged  (e: RadioValueChangedEvent) { 
    const value = e.value;
    this.chosenDisplayType.set(value);
    this.reload$.next();
  }
  milestones$ = combineLatest([this.projectId$, this.reload$])
  .pipe(
    filter(([id]) => !!id),
    switchMap(([id]) => {
      const params: Record<string, any> = {

      }

      if (id) {
        params['project_id'] = id;
      }

      if (this.chosenDisplayType() === 'incompleted') {
        params['completed'] = false;
      }

      if (this.chosenDisplayType() === 'completed') {
        params['completed'] = true;
      }

      // if (this.chosenDisplayType() === 'all') {
      //   params['completed'] = true;
      // }

      return this.http.get<any>('/milestones', { params: params }).pipe(
        map(res => res.data || [])
      )
    }
    )
  )
  .pipe(tap(milestones => {
    this.showMilestoneCharts.set(milestones.length > 0);
  }));

  overview$ = combineLatest([this.projectId$, this.reload$])
  .pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get<ProjectOverviewResponse>(`/projects/${id}/overview`).pipe(
        map(res => res.data)
      )
    )
  )
  .pipe(tap(overview => {
    this.showMilestoneCharts.set(!!overview);
  }));


  
  projectId: string | null = null;
  projectLookupDataSource = this.milestonesLookupDataSourceService.createProjectLookupDataSource();

  constructor() {
    effect(() => {
      const projectId = this.inputProjectId();
      this.projectId = projectId;
      this.projectId$.next(projectId);

      
      if (projectId) {
        this.projectService.getParticipantsInfo(projectId).then(info => {
          this.participantInfo.set(info);
        }).catch(error => {
          console.error('Error fetching participants info:', error);
          this.participantInfo.set(null);
        });
      } else {
        this.participantInfo.set(null);
      }
    });
  }
  
  
  
  
  cumulativeWorkingDayDeltaData = combineLatest([this.projectId$, this.reload$]).pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get('/projects/get-cumulative-working-day-delta', { params: { project_id: id! } }).pipe(
        map((res: any) => {
          const data = res.data?.map((item: {
            estimation_date: string;
            delta: number;
          }) => ({
            x: new Date(item.estimation_date),
            delta: item.delta,
          })) || [];

          if (data.length > 0) {
            const lastDelta = Number(data[data.length - 1].delta);
            if (!Number.isNaN(lastDelta)) {
              this.cumulativeWorkingDayDeltaRangeStart = lastDelta - 15;
              this.cumulativeWorkingDayDeltaRangeEnd = lastDelta + 15;
            }
          }

          return data;
        })
      )
    )
  );

  /**
   * 專案層級收斂圖的資料串流。
   *
   * 流程說明：
   * 1. 監聽專案切換與手動重新載入事件。
   * 2. 針對目前專案呼叫 `/project-acceptance-time-convergence`。
   * 3. 由回傳的 metadata 更新最近 demo 工作大項資訊。
   * 4. 將 `histories` 轉成圖表可用資料點（Date 物件）。
   * 5. 依 `complete_date` 的最小/最大值推算 `convergenceStartDate`/
   *    `convergenceEndDate`，並在前後各延伸 3 個月做為視覺範圍。
   */
  projectConvergenceData = combineLatest([this.projectId$, this.reload$])
  .pipe(
    filter(([id]) => !!id),
    switchMap(([id]) =>
      this.http.get<{ data: { 
        earliest_milestone_name: string; 
        earliest_milestone_complete_date: string;
        histories: MilestoneEstimationHistoryData[] 
      } }>('/project-acceptance-time-convergence', { params: { project_id: id! } })
      .pipe(
        tap(res => {
        //   "earliest_milestone_complete_date": "2026-06-17 00:00:00+08",
        // "earliest_milestone_name": "Milestone-Project-User-0-6-1-1",
          const closestMilestoneName = res.data?.earliest_milestone_name || '未知工作大項';
          const closestMilestoneCompleteDate = res.data?.earliest_milestone_complete_date || '';
          this.closestDemoMilestone.set(closestMilestoneName);
          this.closestDemoMilestoneDate.set(closestMilestoneCompleteDate);
        })
      )
      .pipe(
        map((res: any) => {
          const histories = res.data?.histories?.map((item: MilestoneEstimationHistoryData) => ({
            x: new Date(item.estimation_date),
            complete_date: new Date(item.complete_date),
            optimistic_estimated_complete_date: new Date(item.optimistic_estimated_complete_date),
            pessimistic_estimated_complete_date: new Date(item.pessimistic_estimated_complete_date),
            left_milestones: item.left_milestones,
          })) || [];

          if (histories.length > 0) {
            const lastLeftMilestones = Number(histories[histories.length - 1].left_milestones);
            if (!Number.isNaN(lastLeftMilestones)) {
              this.leftMilestoneRangeStart = Math.max(0, lastLeftMilestones - 15);
              this.leftMilestoneRangeEnd = lastLeftMilestones + 15;
            }
          }

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

          return histories;
        })
      )
    )
  );




  
  participantInfo = signal<{ 
    count: number; 
    avg_excecutor_time_ratio: number;
    sum_estimated_workdays: number;
  } | null>(null);

  projectChanged(e: ValueChangedEvent) {
    const projectId = e.value ?? null;
    this.projectId = projectId;
    this.projectId$.next(projectId);
    this.projectService.getParticipantsInfo(projectId!).then(info => {
      this.participantInfo.set(info);
    }).catch(error => {
      console.error('Error fetching participants info:', error);
    });
  }



  loading = signal(false);
  convergenceStartDate = new Date();
  convergenceEndDate = new Date();
  leftMilestoneRangeStart = 0;
  leftMilestoneRangeEnd = 15;
  cumulativeWorkingDayDeltaRangeStart = -15;
  cumulativeWorkingDayDeltaRangeEnd = 15;
  /**
   * 更新專案相關的工作大項資訊，包含每個工作大項的估算歷史資料，以及專案整體的估算歷史資料。
   * 先更新每一個工作大項的預估時間，然後再更新專案的預估時間。
   * @returns 
   */
  updateProjectInfo() {
    if (!this.projectId) {
      return;
    }
    this.showMilestoneCharts.set(false);
    this.loading.set(true);
    this.http.get<any>('/milestones', { params: { project_id: this.projectId } }).pipe(
      map(res => res.data || []),
      switchMap((milestones: Array<{ id: string }>) =>
        from(milestones).pipe(
          concatMap(milestone =>
            this.http.put(`/milestone-acceptance-time-convergence2?milestone_id=${milestone.id}`, {})
          ),
          toArray()
        )
      ),
      switchMap(() =>
        this.http.put(`/project-acceptance-time-convergence?project_id=${this.projectId}`, {})
      )
    ).subscribe(() => {
      this.reload$.next();
      this.showMilestoneCharts.set(true);
      this.loading.set(false);
    });
  }
}