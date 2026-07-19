import { HttpClient } from '@angular/common/http';
import { Component, inject, signal, viewChild } from '@angular/core';
import { DxTabPanelModule } from 'devextreme-angular/ui/tab-panel';
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
import { DxDataGridModule, DxSelectBoxModule, DxToastModule } from 'devextreme-angular';
import { DxPopupModule } from 'devextreme-angular/ui/popup';
import { PlanService } from './service/plans';
import { DependencyDeletingEvent, DependencyInsertingEvent, TaskMovingEvent, TaskUpdatedEvent, TaskDblClickEvent, TaskUpdatingEvent } from 'devextreme/ui/gantt';
import { formatDate } from 'devextreme/localization';
import { TaskEditor } from './task-editor/task-editor';
import { EstimationHistories } from './estimation-histories/estimation-histories';
import { PlanTaskMetricsHistories } from './plan-task-metrics-histories/plan-task-metrics-histories';
import { PlanMilestoneMetricsHistories } from './plan-milestone-metrics-histories/plan-milestone-metrics-histories';
import { List } from './list/list';
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
    TaskEditor,
    EstimationHistories,
    PlanTaskMetricsHistories,
    PlanMilestoneMetricsHistories,
    List,
    DxTabPanelModule
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




  ngOnInit() {
    (window as any).plansComponent = this;
  }

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
  selectedPlanId = signal<string | null>(null);
  planHistoryRefreshToken = signal<number>(0);
  metricsHistoriesRefreshToken = signal<number>(0);

  onProjectChanged = (e: ValueChangedEvent) => {
    this.planId$.next(e.value);
    this.selectedPlanId.set(e.value);
  }


  onPlanChanged = (e: SelectBoxValueChangedEvent) => {
    this.planId$.next(e.value);
    this.selectedPlanId.set(e.value);
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



  _data = []

  toastVisible = signal<boolean>(false);
  toastMessage = signal<string>('');
  toastType = signal<'info' | 'success' | 'warning' | 'error'>('info');

  data$ = this.planId$.pipe(
    switchMap(planId => {
      if (!planId) {
        return Promise.resolve([]);
      }

      // const isCorrected = this.isCorrected();
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
          const result = data.data.map((task: any) => {
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
              scheduled_start_date: task.scheduled_start_date,
              estimatedWorkdays: task.estimatedWorkdays,
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
          this._data = result;
          return result;
        })
      ).pipe(tap((items) => {
        const earliestStartDateItem = items
          .filter((item: any) => item.start !== null)
          .reduce((earliest: any, item: any) => {
            if (!earliest) {
              return item;
            }

            return item.start < earliest.start ? item : earliest;
          }, null);

        if (earliestStartDateItem) {
          this.gantt()?.instance.scrollToDate(earliestStartDateItem.start);
        }
        this.planHistoryRefreshToken.update((value) => value + 1);
        this.metricsHistoriesRefreshToken.update((value) => value + 1);
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
    if (
      e.newValues.start === undefined || e.newValues.end === undefined) {
      e.cancel = true;
      return
    }
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
    this.popupTask.set(null);
    this.planId$.next(this.planId$.value);
  }

  onTaskDblClick(e: TaskDblClickEvent) {
    console.log('Task double-clicked:', e);
    const taskId = e.key;

    const task = this._data.find((t: any) => t.id === taskId);
    if (!task) {
      this.toastMessage.set('Task not found');
      this.toastType.set('error');
      this.toastVisible.set(true);
      return;
    }



    e.cancel = true; // Prevent the default double-click behavior (e.g., opening the task editor)
    console.log('Task double-clicked:', task);
    this.popupTask.set(task);
    this.popupVisible.set(true);
  }
}