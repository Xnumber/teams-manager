import { Component, input, signal } from '@angular/core';
import { DxButtonModule, DxDataGridModule, DxPopupModule, DxSelectBoxModule } from 'devextreme-angular';
import { CellPreparedEvent, EditorPreparingEvent, InitNewRowEvent, RowPreparedEvent } from 'devextreme/ui/data_grid';
import { JBTask } from '../../pages/tasks/typing';
import { DataSource, DataSourceOptions } from 'devextreme-angular/common/data';
import { editingPopupProps } from './form/config';
import { TasksDataSourceService } from './tasks-data-source.service';
import { LookupDataSourceService } from './lookup-data-source.service';
import { Auth } from '../../services/auth';
import { ChangeLinePipe } from '../../pipes/change-line-pipe';
import { CompleteTaskButton } from '../complete-task-button/complete-task-button';
import { ProjectMilestonesLookupDataSourceService } from '../../pages/projects/project-milestone-lookup-data-source.service';
import { FocusInEvent } from 'devextreme/ui/select_box';

@Component({
  selector: 'app-personal-tasks',
  imports: [
    DxDataGridModule,
    DxPopupModule,
    DxButtonModule,
    DxSelectBoxModule,
    ChangeLinePipe,
    CompleteTaskButton
  ],
  providers: [LookupDataSourceService, TasksDataSourceService, ProjectMilestonesLookupDataSourceService],
  templateUrl: './personal-tasks.html',
  styleUrl: './personal-tasks.scss',
})
export class PersonalTasks {
  title = input<string>('詳細');
  popupVisible = signal(false);
  editingPopupProps = editingPopupProps
  allTasks: DataSource<JBTask>;
  // selectedCreatorId: string | null;
  thisWeekTasks: DataSource<JBTask>;
  nextWeekTasks: DataSource<JBTask>;
  historyTasks: DataSource<JBTask>;
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
  thisWeekTaskCount = signal(0);
  nextWeekTaskCount = signal(0);


  allowUpdating = signal(true);
  allowAdding = signal(true);


  userId: string | null = null;

  milestoneDataSource;
  constructor(
    private tasksDataSourceService: TasksDataSourceService,
    private lookupDataSourceService: LookupDataSourceService,
    private projectMilestonesLookupDataSourceService: ProjectMilestonesLookupDataSourceService,
    private auth: Auth
  ) {
    this.userId = this.auth.getUserId();
    this.milestoneDataSource = this.projectMilestonesLookupDataSourceService.createMilestoneLookupDataSource();
    // creators 改為 DevExtreme DataSource，串接 /users/User
    this.executors = this.lookupDataSourceService.createUserDataSource();
    this.creators = this.lookupDataSourceService.createUserDataSource();
    this.mentors = this.lookupDataSourceService.createUserDataSource();
    // this.selectedCreatorId = null;
    this.projects = this.lookupDataSourceService.createProjectDataSource();
    this.taskTypes = this.lookupDataSourceService.createTaskTypeDataSource();
    this.columns = [
      // Columns are defined in template
    ];
    // Use TasksDataSourceService for allTasks DataSource
    this.allTasks = this.tasksDataSourceService.createAllTasksDataSource();
    this.thisWeekTasks = this.tasksDataSourceService.createThisWeekTasksDataSource();
    this.nextWeekTasks = this.tasksDataSourceService.createNextWeekTasksDataSource();
    this.historyTasks = this.tasksDataSourceService.createHistoryTasksDataSource();

    this.taskStatus = this.lookupDataSourceService.createTaskStatusDataSource();

    this.allTasks.store().on('loaded', (e: any) => {
      console.log('All tasks data source changed', e);
      if (e.totalCount) {
        this.allTaskCount.set(e.totalCount);
      }
    });
    this.thisWeekTasks.store().on('loaded', (e: any) => {
      if (typeof e.totalCount === 'number') {
        this.thisWeekTaskCount.set(e.totalCount);
      }
    });
    this.nextWeekTasks.store().on('loaded', (e: any) => {
      if (typeof e.totalCount === 'number') {
        this.nextWeekTaskCount.set(e.totalCount);
      }
    });
    this.setPersonalTasksDefaultFilters();
  }

  showAllTasks = signal(false);

  setPersonalTasksDefaultFilters = () => {
    this.thisWeekTasks.filter([['executor_id', '=', this.userId], ['mentor_id', '=', this.userId]]);
    this.nextWeekTasks.filter([['executor_id', '=', this.userId], ['mentor_id', '=', this.userId]]);
    this.historyTasks.filter([['executor_id', '=', this.userId], ['mentor_id', '=', this.userId]]);
  }

  onExecutorChange = () => {
    this.setPersonalTasksDefaultFilters();
    this.thisWeekTasks.reload();
    this.nextWeekTasks.reload();
    this.auth.getUserId() === this.userId ? this.allowAdding.set(true) : this.allowAdding.set(false);
  }


  onAllRowUpdated = () => {
    this.thisWeekTasks.reload();
    this.nextWeekTasks.reload();
  }

  onAddTask = () => {
    this.setPersonalTasksDefaultFilters();
    this.allTasks.reload();
    this.thisWeekTasks.reload();
    this.nextWeekTasks.reload();
  }

  displayProgress = (row: JBTask) => {
    return `${Math.round(row.progress * 100)}%`;
  }

  onThisWeekNewTask = (e: InitNewRowEvent<JBTask>) => {
    e.data.this_week = true; // Set this_week to true for tasks created in the "This Week" grid
    // Logic to create a new task for this week
    // 新增後 reload 所有 DataGrid
    this.allTasks.reload();
    this.thisWeekTasks.reload();
    this.nextWeekTasks.reload();
  }

  onNextWeekInserted = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.thisWeekTasks.reload();
  }

  onNextWeekUpdated = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.thisWeekTasks.reload();
  }

  onThisWeekInserted = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.nextWeekTasks.reload();
  }
  onThisWeekUpdated = () => {
    // 新增工作後 reload 所有 DataGrid
    this.allTasks.reload();
    this.nextWeekTasks.reload();
  }
  onNextWeekNewTask = (e: InitNewRowEvent<JBTask>) => {
    e.data.this_week = false; // Set this_week to false for tasks created in the "Next Week" grid
    // Logic to create a new task for next week
    // 新增後 reload 所有 DataGrid
    this.allTasks.reload();
    this.thisWeekTasks.reload();
    this.nextWeekTasks.reload();
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

    // if (e.rowType === "data") {
    //   e.rowElement.style.height = '100px';
    // }
  }

  onCellPrepared = (e: CellPreparedEvent) => {
    if (e.rowType === "data" && (e.column.dataField === "description" || e.column.dataField === "project_name")) {
      e.cellElement.style.whiteSpace = "normal";
      e.cellElement.style.wordBreak = "break-word";
    }
  }

  reloadAllGrids = () => {
    this.allTasks.reload();
    this.thisWeekTasks.reload();
    this.nextWeekTasks.reload();
    this.historyTasks.reload();
  }

  onFocusIn = (e: FocusInEvent) => {
    e.component.getDataSource().reload();
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
}
