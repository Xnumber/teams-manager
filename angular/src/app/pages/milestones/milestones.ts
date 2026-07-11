import { Component, inject, signal, viewChild } from '@angular/core';
import { DxDataGridModule, DxRadioGroupModule } from 'devextreme-angular';
import DataSource, { DataSourceOptions } from 'devextreme/data/data_source';
import { MilestonesDataSourceService } from './milestones-data-source.service';
import { MilestonesLookupDataSourceService } from './lookup-data-source.service';
import { Milestone } from '../../types/milestone';
import { Column, EditorPreparingEvent, InitNewRowEvent, RowExpandedEvent } from 'devextreme/ui/data_grid';
import { Auth } from '../../services/auth';
import { TasksDataSourceService } from './tasks-data-source.service';
import { TaskLookupDataSourceService } from './task-lookup-data-source.service';
import { JBTask } from '../tasks/typing';
import { ChangeLinePipe } from '../../pipes/change-line-pipe';
import { CompleteTaskButton } from '../../components/complete-task-button/complete-task-button';
import { CommonModule } from '@angular/common';
import type { DxoDataGridRowDraggingComponent } from 'devextreme-angular/ui/data-grid/nested';
import { lastValueFrom } from 'rxjs';
import { HttpClient } from '@angular/common/http';
import { FocusInEvent } from 'devextreme/ui/select_box';
import { MilestonesService } from '../../services/milestones';
import { ValueChangedEvent as RadioValueChangedEvent } from 'devextreme/ui/radio_group';
import { CompleteMilestoneButton } from '../../components/complete-milestone-button /complete-milestone-button';
import { DependenciesEditCell } from '../../components/dependencies-edit-cell/dependencies-edit-cell';

@Component({
  selector: 'app-milestones',
  imports: [
    DxDataGridModule, 
    ChangeLinePipe, 
    CompleteTaskButton,
    DxRadioGroupModule,
    CompleteMilestoneButton,
    DependenciesEditCell
  ],
  providers: [
    MilestonesDataSourceService,
    MilestonesLookupDataSourceService,
    TaskLookupDataSourceService,
    CommonModule
  ],
  templateUrl: './milestones.html',
  styleUrl: './milestones.scss',
})
export class Milestones {







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
    this.milestonesDataSource.filter(['completed', value === 'completed']);
    this.milestonesDataSource.reload();
  }

  milestonesDataSource: DataSource<Milestone>;
  projectLookup: any;
  teamLookup: any;
  tenantLookup: any;
  columns: any[] = [];
  tasksFilterDataSource: DataSource<JBTask> | null = null;
  taskColumns: Column[] = [];
  http = inject(HttpClient);

  dependencyMilestoneLookupDataSource: DataSourceOptions | null = null;
  grid = viewChild('grid') as any;




  displayProgress = (row: JBTask) => {
    return `${Math.round(row.progress * 100)}%`;
  }
  constructor(
    private auth: Auth,
    private milestonesDataSourceService: MilestonesDataSourceService,
    private milestonesLookupDataSourceService: MilestonesLookupDataSourceService,
    protected tasksDataSourceService: TasksDataSourceService,
    private taskLookupDataSourceService: TaskLookupDataSourceService,
    private milestonesService: MilestonesService
  ) {
    this.milestonesDataSource = this.milestonesDataSourceService.createMilestonesDataSource();

    const userDataSource = this.taskLookupDataSourceService.createUserDataSource();
    const projectDataSource = this.taskLookupDataSourceService.createProjectDataSource();
    const taskTypeDataSource = this.taskLookupDataSourceService.createTaskTypeDataSource();
    const taskStatusDataSource = this.taskLookupDataSourceService.createTaskStatusDataSource();
    const milestoneLookupDataSource = this.taskLookupDataSourceService.createMilestoneLookupDataSource();


    this.taskColumns = [
      { dataField: 'priority', caption: '優先級', width: 75, dataType: 'number', formItem: { visible: false } },
      { dataField: 'progress', caption: '進度', width: 75, dataType: 'number', format: 'percent', formItem: { editorOptions: { min: 0, max: 1, step: 0.01, format: 'percent' } }, calculateDisplayValue: this.displayProgress },
      { dataField: 'name', caption: '工作名稱', validationRules: [{ type: 'required' }] },
      { dataField: 'estimated_workdays', width: 75, caption: '天數', dataType: 'number', editorOptions: { min: 0 } },
      { dataField: 'excecutor_time_ratio', width: 75, caption: '時間比例', dataType: 'number', format: 'percent', formItem: { editorOptions: { min: 0.05, format: 'percent', max: 1, step: 0.05 } } },
      { dataField: 'description', caption: '描述', dataType: 'string', cellTemplate: 'descriptionCellTemplate', hidingPriority: 1 },
      {
        dataField: 'milestone_id', caption: '工作大項',
        visible: false,
        lookup: { dataSource: milestoneLookupDataSource, valueExpr: 'id', displayExpr: 'name' },
        formItem: { editorOptions: { disabled: true } },
        validationRules: [{ type: 'required' }],
      },
      { dataField: 'this_week', visible: false, formItem: { visible: false } },
      // { dataField: 'project_name', caption: '專案', formItem: { visible: false } },
      { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
      {
        dataField: 'status_id', caption: '狀態',
        width: 75,
        lookup: { dataSource: taskStatusDataSource, valueExpr: 'id', displayExpr: 'name' },
        validationRules: [{ type: 'required' }],
      },
      // {
      //   dataField: 'creator_id', caption: '建立者', visible: false,
      //   lookup: { dataSource: userDataSource, valueExpr: 'id', displayExpr: 'name' },
      //   formItem: { visible: false },
      // },
      // { dataField: 'creator_name', caption: '建立者', hidingPriority: 4, formItem: { visible: false } },


      {
        dataField: 'project_id', caption: '專案', visible: false,
        lookup: { dataSource: projectDataSource, valueExpr: 'id', displayExpr: 'name' },
        formItem: { editorOptions: { disabled: true } },
        validationRules: [{ type: 'required' }],
      },
      {
        dataField: 'task_type_id', caption: '工作類型', visible: false,

        lookup: { dataSource: taskTypeDataSource, valueExpr: 'id', displayExpr: 'name' },
      },
      { dataField: 'task_type_name', width: 75, caption: '工作類型', formItem: { visible: false } },
      // {
      //   dataField: 'status_last_changed', caption: '狀態變更時間', dataType: 'date',
      //   format: 'yyyy/MM/dd', hidingPriority: 7,
      //   formItem: { visible: false },
      // },

      {
        dataField: 'executor_ids', caption: '執行者', hidingPriority: 1,
        formItem: { editorType: 'dxTagBox' },
        validationRules: [{ type: 'required' }],
        editorOptions: { dataSource: userDataSource, valueExpr: 'id', displayExpr: 'name', showSelectionControls: true, applyValueMode: 'useButtons' },
      },
      {
        dataField: 'mentor_ids', caption: '指導者', hidingPriority: 3,
        formItem: { editorType: 'dxTagBox', visible: false },
        visible: false,
        editorOptions: { dataSource: userDataSource, valueExpr: 'id', displayExpr: 'name', showSelectionControls: true, applyValueMode: 'useButtons' },
      },
      // {
      //   dataField: 'scheduled_completion_date', caption: '預定完成日期', dataType: 'date',
      //   visible: false, formItem: { visible: false },
      // },
      // {
      //   dataField: 'excecutor_time_ratio', caption: '參與者投入時間百分比', dataType: 'number',
      //   visible: false,
      //   formItem: { editorOptions: { min: 0, format: 'percent', max: 1, step: 0.05 }, visible: false },
      // },
      // {
      //   dataField: 'scheduled_start_date', caption: '開始日期', dataType: 'date',
      //   format: 'yyyy/MM/dd', hidingPriority: 5,
      //   formItem: { visible: false },
      // },
      // {
      //   dataField: 'scheduled_end_date', caption: '結束日期', dataType: 'date',
      //   format: 'yyyy/MM/dd', hidingPriority: 6,
      //   formItem: { visible: false },
      // },
      {
        dataField: 'completion_date', caption: '完成日期', dataType: 'date',
        visible: false, formItem: { visible: false },
      },
      {
        dataField: 'remark', caption: '備註', hidingPriority: 0,
        formItem: { colSpan: 2, editorType: 'dxTextArea', visible: true },
      },
      {
        type: 'buttons',
        buttons: [{ name: 'edit' }, { template: 'completeButtonTemplate' }]
      },

      // { dataField: 'completed', caption: '完成', dataType: 'boolean' },
    ];
    this.projectLookup = this.milestonesLookupDataSourceService.createProjectLookupDataSource();
    this.dependencyMilestoneLookupDataSource = this.milestonesLookupDataSourceService.createDependencyMilestoneLookupDataSource();
    this.teamLookup = this.milestonesLookupDataSourceService.createTeamLookupDataSource();
    this.tenantLookup = this.milestonesLookupDataSourceService.createTenantLookupDataSource();

    this.columns = [
      { dataField: 'name', caption: '工作大項名稱', validationRules: [{ type: 'required' }] },
      { dataField: 'is_for_demo', caption: '展示用', width: 75, dataType: 'boolean' },
      {
        dataField: 'estimated_start_date', caption: '預計開始日期', dataType: 'date', format: 'yyyy/MM/dd',
      },
      {
        dataField: 'project_id', caption: '專案',
        lookup: {
          dataSource: this.projectLookup, valueExpr: 'id', displayExpr: 'name'
        },
        // formItem: {
        //   editorOptions: {
        //     onValueChanged: (e: any) => {
        //       this.milestonesLookupDataSourceService.projectId = e.value;
        //       e.setValue(e.value); // 觸發值變更以刷新依賴工作大項的選項
        //     }
        //   }
        // },
        validationRules: [{ type: 'required' }]
      },
      // {
      //   dataField: 'dependency_milestone_id',
      //   caption: '依賴工作大項',
      //   lookup: {
      //     dataSource: this.dependencyMilestoneLookupDataSource, valueExpr: 'id', displayExpr: 'name'
      //   },
      //   editorOptions: {
      //     onFocusIn: (e: FocusInEvent) => {
      //       e.component.getDataSource().reload();
      //     }
      //   }
      // },

      { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
      { dataField: 'description', caption: '描述', editorType: 'dxTextArea', formItem: { colSpan: 2, editorOptions: { height: 100 } }, validationRules: [{ type: 'required' }] },
      { dataField: 'team_id', caption: '團隊', lookup: { dataSource: this.teamLookup, valueExpr: 'id', displayExpr: 'name' }, validationRules: [{ type: 'required' }] },
      { dataField: 'tenant_id', caption: '組織', lookup: { dataSource: this.tenantLookup, valueExpr: 'id', displayExpr: 'name' }, validationRules: [{ type: 'required' }] },
      // { dataField: 'start_date', caption: '開始日期', dataType: 'date', formItem: { visible: false } },
      // { dataField: 'end_date', caption: '結束日期', dataType: 'date', formItem: { visible: false } },
      { dataField: 'remark', caption: '備註', editorType: 'dxTextArea', formItem: { colSpan: 2, editorOptions: { height: 100 } } },
      { dataField: 'concurrency_stamp', caption: 'Concurrency', visible: false, formItem: { visible: false } },
      { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false, formItem: { visible: false } },
      {
        dataField: 'completed', caption: '完成', dataType: 'boolean'
      },
      {
        dataField: 'dependencies', caption: '依賴', dataType: 'boolean', editCellTemplate: "dependenciesEditCellTemplate", visible: false
      },
      {
        type: 'buttons',
        buttons: [
          { name: 'edit' },
          {
            template: 'complete'
          }
        ]
      },
    ];
  }


  onEditorPreparing = (e: EditorPreparingEvent) => {
    if (e.dataField === "project_id") {

      const defaultHandler = e.editorOptions.onValueChanged;

      e.editorOptions.onValueChanged = (args: any) => {

        // DataGrid Editor 的值
        e.setValue(args.value);
        this.milestonesLookupDataSourceService.projectId = args.value;
        if (defaultHandler) {
          defaultHandler(args);
        }
      };
    }
  }


  onInitNewRow(e: InitNewRowEvent<Milestone>) {
    const tenantId = this.auth.getTenantId();
    const teamId = this.auth.getTeamId();
    if (tenantId) {
      e.data.tenant_id = tenantId; // 預設租戶ID
    }
    if (teamId) {
      e.data.team_id = teamId; // 預設團隊ID
    }
  }

  onRowExpanded(e: RowExpandedEvent) {
    const id = e.key['id'];
    if (id) {
      this.tasksFilterDataSource = this.tasksDataSourceService.createTasksFilterDataSource(id);
    } else {
      this.tasksFilterDataSource = null;
    }
  }

  onTasksReorder: typeof DxoDataGridRowDraggingComponent.prototype.onReorder = (event) => {
    event.component.beginCustomLoading("正在重新排序...");
    const id = event.itemData.id;
    const milestoneId = event.itemData.milestone_id;
    const index = event.toIndex;

    this.reprioritizeTask(id, milestoneId, index + 1)
      .then(() => event.component.refresh())
      .catch((error: unknown) => {
        console.error('Task reprioritize failed:', error);
        event.component.refresh();
      })
      .finally(() => {
        event.component.endCustomLoading();
      });
  }
  
  reprioritizeTask(taskId: string, milestoneId: string, newPriority: number): Promise<void> {
    return lastValueFrom(
      this.http.put<{ result: string; message?: string }>(`/task/reprioritize`, {
        task_id: taskId,
        milestone_id: milestoneId,
        priority: newPriority,
      })
    ).then((res) => {
      if (res?.result !== 'ok') {
        throw new Error(res?.message || 'Reprioritize failed');
      }
    });
  }

  onInitNewTaskRow(e: InitNewRowEvent<JBTask>, milestoneId: string, projectId: string) {
    e.data.milestone_id = milestoneId; // 預設工作大項ID
    e.data.project_id = projectId; // 預設專案ID
    e.data.excecutor_time_ratio = 1;
    const user_id = this.auth.getUserId();
    if (user_id) {
      console.log('Setting default executor_id to current user:', user_id);
      e.data.executor_ids = [user_id]; // 預設建立者ID
    }
  }

  // onMilestoneReorder: typeof DxoDataGridRowDraggingComponent.prototype.onReorder = (event) => {
  //   event.component.beginCustomLoading("正在重新排序...");
  //   const id = event.itemData.id;
  //   const tenantId = event.itemData.tenant_id;
  //   const newPriority = event.toIndex + 1;

  //   this.milestonesService.reprioritize(id, newPriority, tenantId)
  //     .then(() => event.component.refresh())
  //     .catch((error: unknown) => {
  //       console.error('Milestone reprioritize failed:', error);
  //       event.component.refresh();
  //     })
  //     .finally(() => {
  //       event.component.endCustomLoading();
  //     });
  // }
}