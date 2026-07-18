import { HttpClient } from '@angular/common/http';
import { Component, inject, signal, viewChild } from '@angular/core';
import { DxGanttComponent } from 'devextreme-angular/ui/gantt';
import { DxGanttModule } from 'devextreme-angular';
import { Dependency, Resource, ResourceAssignment, Task } from './service/data';
import { tap, map, switchMap, catchError, of, merge } from 'rxjs';
import { BehaviorSubject, Subject } from 'rxjs';
import { Data } from './service/data';
import { CommonModule } from '@angular/common';
import { DxButtonModule } from 'devextreme-angular/ui/button';
import { ProjectsService } from '../../services/projects';
import { ValueChangedEvent } from 'devextreme/ui/tag_box';
import { ValueChangedEvent as SelectBoxValueChangedEvent } from 'devextreme/ui/select_box';
import { ValueChangedEvent as CheckboxValueChangedEvent } from 'devextreme/ui/check_box';
import { DxTagBoxModule } from 'devextreme-angular/ui/tag-box';
import { DxCheckBoxModule } from 'devextreme-angular/ui/check-box';
import { Column } from 'devextreme/ui/data_grid';
import { DxChartModule, DxDataGridModule, DxSelectBoxModule, DxToastModule } from 'devextreme-angular';
import { DxPopupModule } from 'devextreme-angular/ui/popup';
import { PlanService } from './service/plans';
import { DependencyDeletingEvent, DependencyInsertingEvent, TaskMovingEvent, TaskUpdatedEvent, TaskDblClickEvent, TaskUpdatingEvent } from 'devextreme/ui/gantt';
import { formatDate } from 'devextreme/localization';
@Component({
  selector: 'app-plans',
  imports: [
    DxGanttModule,
    DxButtonModule,
    CommonModule,
    DxTagBoxModule,
    DxCheckBoxModule,
    DxPopupModule,
    DxDataGridModule,
    DxSelectBoxModule,
    DxToastModule,
    DxChartModule
  ],
  templateUrl: './plans.html',
  styleUrl: './plans.scss',
  providers: [
    Data,
    ProjectsService,
    PlanService
  ]
})
export class Plans {
  isCorrected = signal<boolean>(true);

  planService = inject(PlanService);
  // projectService = inject(ProjectsService);

  plansDataSource = this.planService.createPlansDataSource();

  // projectsDataSource = this.projectService.createProjectLookupDataSource();





  http = inject(HttpClient);
  projectService = inject(ProjectsService);
  projects = this.projectService.createProjectLookupDataSource();
  plans = this.planService.createPlansDataSource();

  planColumns: Column[] = [
    {
      dataField: 'id',
      caption: 'ID',
      width: 100,
      allowEditing: false,
      visible: false,
      formItem: { visible: false }
    },
    {
      dataField: 'name',
      caption: '計畫名稱',
      width: 150,
      validationRules: [{ type: 'required' }],
      formItem: { colSpan: 2 }
    },


    {
      dataField: 'start_date',
      caption: '開始日期',
      dataType: 'date',
      width: 120,
      format: 'yyyy-MM-dd',
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'end_date',
      caption: '結束日期',
      dataType: 'date',
      width: 120,
      format: 'yyyy-MM-dd',
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'project_ids',
      caption: '專案',
      width: 150,
      lookup: {
        dataSource: this.projects,
        valueExpr: 'id',
        displayExpr: 'name'
      },
      validationRules: [{ type: 'required' }],
      formItem: { colSpan: 2, editorType: 'dxTagBox' }
    },
    {
      dataField: 'description',
      caption: '說明',
      formItem: { editorType: 'dxTextArea', colSpan: 2 },
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'created_at',
      caption: '建立時間',
      dataType: 'datetime',
      width: 140,
      allowEditing: false,
      format: 'yyyy-MM-dd HH:mm:ss',
      visible: false,
      formItem: { visible: false }
    },
    {
      dataField: 'status',
      caption: '狀態',
      width: 100,
      lookup: {
        dataSource: [
          { id: 'draft', text: '草稿' },
          { id: 'active', text: '進行中' },
          { id: 'completed', text: '已完成' },
          { id: 'cancelled', text: '已取消' }
        ],
        valueExpr: 'id',
        displayExpr: 'text'
      },
      validationRules: [{ type: 'required' }]
    }
  ]


  onIsCorrectedChanged = (e: CheckboxValueChangedEvent) => {
    this.isCorrected.set(e.value);
    this.planId$.next(this.planId$.value);
  }

  private planId$ = new BehaviorSubject<string | null>(null);
  private refreshPlanHistory$ = new Subject<void>();

  onProjectChanged = (e: ValueChangedEvent) => {
    this.planId$.next(e.value);
  }

  refreshPlanHistory() {
    this.refreshPlanHistory$.next();
  }


  onPlanChanged = (e: SelectBoxValueChangedEvent) => {
    this.planId$.next(e.value);
  }

  tasks: Task[];

  dependencies: Dependency[];

  resources: Resource[];

  resourceAssignments: ResourceAssignment[];




  delayedTasks = signal<any[]>([]);
  totalDelayedDays = signal<number>(0);
  latestTaskEnd = signal<string | null>(null);

  popupVisible = signal<boolean>(false);
  popupTask = signal<any | null>(null);





  toastVisible = signal<boolean>(false);
  toastMessage = signal<string>('');
  toastType = signal<'info' | 'success' | 'warning' | 'error'>('info');

  data$ = this.planId$.pipe(
    switchMap(planId => {
      if (!planId) {
        return Promise.resolve([]);
      }

      const isCorrected = this.isCorrected();
      // const url = isCorrected ? '/corrected-gantt-chart' : '/gantt-chart';
      const url = '/corrected-gantt-chart';

      return this.http.get(url, {
        params: { plan_id: planId },
      }).pipe(
        tap((data: any) => {
          this.dependencies = data.dependencies;
          console.log('dependencies', this.dependencies);
          this.delayedTasks.set(data.delayedTasks || []);
          const totalDelayedDays = data.delayedTasks?.reduce((acc: number, task: any) => acc + (task.delayDays || 0), 0) || 0;
          this.totalDelayedDays.set(totalDelayedDays);
          this.latestTaskEnd.set(data.latestTaskEnd || null);
          // this.latestTaskEnd.set('2026-07-31'); // Set a default value for latestTaskEnd
        }),
        map((data: any) => {
          return data.data.map((task: any) => {
            // const start = task.start ? new Date(`${task.start}T00:00:00.000+08:00`) : null;
            // const end = task.end ? new Date(`${task.end}T24:00:00.000+08:00`) : null;
            const start = task.start ? new Date(`${task.start}T00:01:00.000+08:00`) : null;
            const end = task.end ? new Date(`${task.end}T23:59:59.999+08:00`) : null;

            let color = undefined;

            if (task.delayed) {
              color = '#ff2e2e';
            }

            if (task.completed) {
              color = '#a6b8a63d';
            }

            if (task.ahead) {
              color = '#00a2ff3d';
            }

            const item = {
              id: task.id,
              parentId: task.parentId,
              title: task.title,
              start: start,
              end: end,
              progress: task.progress * 100,
              workItemType: task.workItemType,
              ahead: task.ahead,
              delayed: task.delayed,
              color
            }

            return item;
          });
        })
      ).pipe(tap((items) => {
        const firstItemWithStartDate = items.find((item: any) => item.start !== null);
        if (firstItemWithStartDate) {
          this.gantt()?.instance.scrollToDate(firstItemWithStartDate.start);
        }
        this.refreshPlanHistory();
      })).pipe(
        catchError((error) => {
          // console.error('Error fetching Gantt chart data:', error);
          this.toastMessage.set(error.error.message || 'An error occurred while fetching data.');
          this.toastType.set('error');
          this.toastVisible.set(true);
          return of([]);
        })
      )
    })
  )

  convergenceStartDate = new Date()
  convergenceEndDate = new Date()

  planHistory$ = this.refreshPlanHistory$.pipe(
    switchMap(() => {
      const planId = this.planId$.value;
      if (!planId) {
        return Promise.resolve([]);
      }
      console.log('planHistory data:', planId);

      return this.planService.getPlanHistory(planId).then(data => {
        console.log('planHistory data:', data);
        const histories = data?.map((item) => ({
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
          // const lastLeftTasks = histories[histories.length - 1].left_tasks;
          // this.leftTasksRangeStart = Math.max(0, lastLeftTasks - 15);
          // this.leftTasksRangeEnd = lastLeftTasks + 15;
        }

        return histories;
        return data;
      }).catch(error => {
        this.toastMessage.set(error.error.message || 'An error occurred while fetching plan history.');
        this.toastType.set('error');
        this.toastVisible.set(true);
        return [];
      });
    })
  );

  service = inject(Data);

  constructor() {
    this.tasks = this.service.getTasks();
    this.dependencies = this.service.getDependencies();
    this.resources = this.service.getResources();
    this.resourceAssignments = this.service.getResourceAssignments();
  }


  gantt = viewChild<DxGanttComponent>("gantt")

  zoomIn() {
    this.gantt()?.instance.zoomIn();
  }

  zoomOut() {
    this.gantt()?.instance.zoomOut();
  }

  // Dependency Type	Supported Values
  // Finish to Start (FS)	0, "0", "FS", "fs"
  // Start to Start (SS)	1, "1", "SS", "ss"
  // Finish to Finish (FF)	2, "2", "FF", "ff"
  // Start to Finish (SF)	3, "3", "SF", "sf"
  
  onDependencyInserting(e: DependencyInsertingEvent) {
    const task = (e as any).taskData ?? (e as any).task ?? null;
    const predecessorId = e.values.predecessorId;
    const successorId = e.values.successorId;

    // If dependency type is not FS (0), call backend API to create dependency
    if (e.values.type != 0) {
      e.cancel = true; // prevent default client-side insertion

      
      
    } else {
      if (!predecessorId || !successorId) {
        this.toastMessage.set('Invalid dependency data');
        this.toastType.set('error');
        this.toastVisible.set(true);
        return;
      }
      const url = `/tasks/${successorId}/dependency`;
      const body = { dependency_id: predecessorId };
      
      this.http.put(url, body)
      .subscribe({
        next: (res: any) => {
          this.toastMessage.set(res?.message || 'Dependency created');
          this.toastType.set('success');
          this.toastVisible.set(true);
          // refresh gantt data
          this.planId$.next(this.planId$.value);
        },
        error: (err: any) => {
          this.toastMessage.set(err?.error?.message || 'Failed to create dependency');
          this.toastType.set('error');
          this.toastVisible.set(true);
        }
      });
      // FS default behavior: allow client insertion (do nothing)
    }
  }

  
  onTaskUpdating(e: TaskUpdatingEvent) {
    console.log('Task updated event:', e);
    const start = e.newValues.start;
    const formattedStart = start ? formatDate(start, 'yyyy-MM-dd') : null;
    if (start) {
      this.http.put(`/tasks/scheduled-start-date`, 
        {
          task_id: e.key, 
          scheduled_start_date: formattedStart
        })
        .subscribe({
          next: (res: any) => {
            this.toastMessage.set(res?.message || 'Task updated');
            this.toastType.set('success');
            this.toastVisible.set(true);
            // // refresh gantt data
            this.planId$.next(this.planId$.value);
          },
          error: (err: any) => {
            this.toastMessage.set(err?.error?.message || 'Failed to update task');
            this.toastType.set('error');
            this.toastVisible.set(true);
          }
        });
    }
  }


  onDependencyDeleting(e: DependencyDeletingEvent) {
    console.log('Deleting dependency:', e);
    const dependencyId = e.key;
    if (!dependencyId) {
      this.toastMessage.set('Invalid dependency data');
      this.toastType.set('error');
      this.toastVisible.set(true);
      return;
    }
    const url = `/tasks/delete-dependency`;
    this.http.put(url, {
      predecessor_id: e.values.predecessorId,
      successor_id: e.values.successorId
    })
      .subscribe({
        next: (res: any) => {
          this.toastMessage.set(res?.message || 'Dependency deleted');
          this.toastType.set('success');
          this.toastVisible.set(true);
          // refresh gantt data
          this.planId$.next(this.planId$.value);
        },
        error: (err: any) => {
          this.toastMessage.set(err?.error?.message || 'Failed to delete dependency');
          this.toastType.set('error');
          this.toastVisible.set(true);
        }
      });
  }





  closePopup() {
    this.popupVisible.set(false);
  }

  onTaskDblClick(e: TaskDblClickEvent) {
    console.log('Task double-clicked:', e);
    

    e.cancel = true; // Prevent the default double-click behavior (e.g., opening the task editor)
    console.log('Task double-clicked:', e.data);
    this.popupTask.set(e.data);
    this.popupVisible.set(true);
  }
}