import { Component, signal } from '@angular/core';
import { DxButtonModule, DxCheckBoxModule, DxDataGridModule, DxSelectBoxModule, DxTextAreaModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { JBTask } from './typing';
import { TasksDataSourceService } from './tasks-data-source.service';
import { CustomStore, DataSourceOptions } from 'devextreme/common/data';
import { LookupDataSourceService } from './lookup-data-source.service';
import { EditorPreparingEvent, InitNewRowEvent, RowPreparedEvent } from 'devextreme/ui/data_grid';
import { TeamTaskContext } from './team-task-context/team-task-context';
import { Auth } from '../../services/auth';
import { editingPopupProps } from './config/form';
import { PersonalTasks } from '../../components/personal-tasks/personal-tasks';
import { CompleteTaskButton } from '../../components/complete-task-button/complete-task-button';
import { Milestone } from '../../types/milestone';
import { MilestonesDataSourceService } from './milestones-data-source.service';
import { ProjectMilestonesLookupDataSourceService } from './project-milestone-lookup-data-source.service';
import { FocusInEvent } from 'devextreme/ui/select_box';
import { DependenciesEditCell } from '../../components/dependencies-edit-cell/dependencies-edit-cell';
import type { WorkItemDependency } from '../../components/dependencies-edit-cell/typing';
import { Task } from '../../services/task';

@Component({
  selector: 'app-tasks',
  imports: [
    DxDataGridModule,
    DxButtonModule,
    DxSelectBoxModule,
    DxCheckBoxModule,
    DxTextAreaModule,
    PersonalTasks,
    CompleteTaskButton,
    DependenciesEditCell
    // TeamTaskContext
  ],
  providers: [LookupDataSourceService, ProjectMilestonesLookupDataSourceService],
  templateUrl: './tasks.html',
  styleUrl: './tasks.scss',
})
export class Tasks {
  editingPopupProps = editingPopupProps
  allTasks: DataSource<JBTask>;
  milestoneDataSource;
  // selectedCreatorId: string | null;
  inCompletedTasks: DataSource<JBTask>;
  completedTasks: DataSource<JBTask>;
  columns: any = [];
  executors: DataSourceOptions;
  creators: DataSourceOptions;
  taskStatus: DataSourceOptions;
  mentors: DataSourceOptions;
  projects: DataSourceOptions;
  taskTypes: DataSourceOptions;

  taskContextVisible = signal(false);
  selectedTask: Partial<JBTask> = {};

  allTaskCount = signal(0);
  uncompletedCount = signal(0);
  nextWeekTaskCount = signal(0);


  allowUpdating = signal(true);
  allowAdding = signal(true);


  userId: string | null = null;
  constructor(
    private taskService: Task,
    private tasksDataSourceService: TasksDataSourceService,
    private lookupDataSourceService: LookupDataSourceService,
    private milestonesDataSourceService: MilestonesDataSourceService,
    private projectMilestonesLookupDataSourceService: ProjectMilestonesLookupDataSourceService,
    private auth: Auth
  ) {
    this.userId = this.auth.getUserId();
    // creators 改為 DevExtreme DataSource，串接 /users/User
    this.executors = this.lookupDataSourceService.createUserDataSource();
    this.creators = this.lookupDataSourceService.createUserDataSource();
    this.mentors = this.lookupDataSourceService.createUserDataSource();
    // this.selectedCreatorId = null;
    this.projects = this.projectMilestonesLookupDataSourceService.createProjectLookupDataSource();
    this.taskTypes = this.lookupDataSourceService.createTaskTypeDataSource();
    this.columns = [
      // Columns are defined in template
    ];
    // Use TasksDataSourceService for allTasks DataSource
    this.allTasks = this.tasksDataSourceService.createAllTasksDataSource();
    this.inCompletedTasks = this.taskService.createTasksDataSource();
    this.completedTasks = this.taskService.createTasksDataSource();

    this.taskStatus = this.lookupDataSourceService.createTaskStatusDataSource();

    this.allTasks.store().on('loaded', (e: any) => {
      console.log('All tasks data source changed', e);
      if (e.totalCount) {
        this.allTaskCount.set(e.totalCount);
      }
    });
    this.inCompletedTasks.store().on('loaded', (e: any) => {
      if (typeof e.totalCount === 'number') {
        this.uncompletedCount.set(e.totalCount);
      }
    });
    this.completedTasks.store().on('loaded', (e: any) => {
      if (typeof e.totalCount === 'number') {
        this.nextWeekTaskCount.set(e.totalCount);
      }
    });

    this.milestoneDataSource = this.projectMilestonesLookupDataSourceService.createMilestoneLookupDataSource();

    this.setPersonalTasksDefaultFilters();
  }

  showAllTasks = signal(false);

  setPersonalTasksDefaultFilters = () => {
    this.inCompletedTasks.filter([['executor_id', '=', this.userId], ['mentor_id', '=', this.userId], ['completed', '=', 'false']]);
    this.completedTasks.filter([['executor_id', '=', this.userId], ['mentor_id', '=', this.userId], ['completed', '=', 'true']]);
    // this.inCompletedTasks.filter(['completed', '=', 'false'])
  }

  onExecutorChange = () => {
    this.setPersonalTasksDefaultFilters();
    this.inCompletedTasks.reload();
    this.completedTasks.reload();
    this.auth.getUserId() === this.userId ? this.allowAdding.set(true) : this.allowAdding.set(false);
  }


  onAllRowUpdated = () => {
    this.inCompletedTasks.reload();
    this.completedTasks.reload();
  }

  onAddTask = () => {
    this.setPersonalTasksDefaultFilters();
    this.allTasks.reload();
    this.inCompletedTasks.reload();
    this.completedTasks.reload();
  }

  displayProgress = (row: JBTask) => {
    return `${Math.round(row.progress * 100)}%`;
  }

  onThisWeekNewTask = (e: InitNewRowEvent<JBTask>) => {
    e.data.this_week = true; // Set this_week to true for tasks created in the "This Week" grid
    // Logic to create a new task for this week
    // 新增後 reload 所有 DataGrid
    this.allTasks.reload();
    this.inCompletedTasks.reload();
    this.completedTasks.reload();
  }

  onNextWeekInserted = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.inCompletedTasks.reload();
  }

  onNextWeekUpdated = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.inCompletedTasks.reload();
  }

  onThisWeekInserted = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.completedTasks.reload();
  }
  onThisWeekUpdated = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.completedTasks.reload();
  }
  onNextWeekNewTask = (e: InitNewRowEvent<JBTask>) => {
    e.data.this_week = false; // Set this_week to false for tasks created in the "Next Week" grid
    // Logic to create a new task for next week
    // 新增後 reload 所有 DataGrid
    this.allTasks.reload();
    this.inCompletedTasks.reload();
    this.completedTasks.reload();
  }

  showTask = (e: any) => {
    console.log('Show task details:', e);
    this.selectedTask = e.row.data;
    this.taskContextVisible.set(true);
  }

  onRowPrepared = (e: RowPreparedEvent) => {
    if (e.rowType === "data" && typeof e.data['progress']) {
      const progress = Math.round((e.data['progress'] * 100)); // Convert to percentage
      const className = `progress-${progress}`
      e.rowElement.classList.add(className);
    }
  }





  onEditorPreparing = (e: EditorPreparingEvent) => {
    if (e.dataField === "project_id") {

      const defaultHandler = e.editorOptions.onValueChanged;
      e.editorOptions.onValueChanged = (args: any) => {
        console.log('Editor preparing for project_id field', e);

        // DataGrid Editor 的值
        e.setValue(args.value);
        this.projectMilestonesLookupDataSourceService.projectId = args.value;
        if (defaultHandler) {
          defaultHandler(args);
        }
      };
    }
  }

  
  onFocusIn = (e: FocusInEvent) => {
    e.component.getDataSource().reload();
  }

  onDependenciesChanged(value: WorkItemDependency[], taskId: number) {
    console.log('Dependencies changed for task', taskId, value);
  }
}