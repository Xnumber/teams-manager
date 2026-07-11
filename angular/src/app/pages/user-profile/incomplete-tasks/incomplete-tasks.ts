import { Component, signal } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import DataSource, { DataSourceOptions } from 'devextreme/data/data_source';
import { DxDataGridModule } from 'devextreme-angular';
import { lastValueFrom } from 'rxjs';
import { JBTask, RawTask } from '../../tasks/typing';
import { Auth } from '../../../services/auth';
import { CompleteTaskButton } from '../../../components/complete-task-button/complete-task-button';
import { LookupDataSourceService } from './lookup-data-source.service';
import { ProjectMilestonesLookupDataSourceService } from './project-milestone-lookup-data-source.service';
import { FocusInEvent } from 'devextreme/ui/select_box';
import { EditorPreparingEvent } from 'devextreme/ui/data_grid';
import { RequestConfirmationButton } from '../../../components/request-confirmation-button/request-confirmation-button';

@Component({
  selector: 'app-incomplete-tasks',
  imports: [
    DxDataGridModule, 
    CompleteTaskButton,
    RequestConfirmationButton
  ],
  templateUrl: './incomplete-tasks.html',
  providers: [LookupDataSourceService, ProjectMilestonesLookupDataSourceService],
  styleUrl: './incomplete-tasks.scss',
})
export class IncompleteTasks {
  executors: DataSourceOptions;
  creators: DataSourceOptions;
  taskStatus: DataSourceOptions;
  mentors: DataSourceOptions;
  projects: DataSourceOptions;
  taskTypes: DataSourceOptions;

  milestoneDataSource: DataSourceOptions;
  incompleteTasks: DataSource<JBTask>;
  taskCount = signal(0);

  constructor(
    private http: HttpClient,
    private auth: Auth,
    private lookupDataSourceService: LookupDataSourceService,
    private projectMilestonesLookupDataSourceService: ProjectMilestonesLookupDataSourceService
  ) {
    const userId = this.auth.getUserId();
    this.incompleteTasks = this.createIncompleteTasksDataSource(userId);

    this.incompleteTasks.store().on('loaded', (e: any) => {
      if (typeof e.totalCount === 'number') {
        this.taskCount.set(e.totalCount);
      }
    });
    this.executors = this.lookupDataSourceService.createUserDataSource();
    this.creators = this.lookupDataSourceService.createUserDataSource();
    this.mentors = this.lookupDataSourceService.createUserDataSource();
    // this.selectedCreatorId = null;
    this.projects = this.projectMilestonesLookupDataSourceService.createProjectLookupDataSource();
    this.taskTypes = this.lookupDataSourceService.createTaskTypeDataSource();


    this.taskStatus = this.lookupDataSourceService.createTaskStatusDataSource();
    this.milestoneDataSource = this.projectMilestonesLookupDataSourceService.createMilestoneLookupDataSource();
  }



  private createIncompleteTasksDataSource(userId: string | null): DataSource<JBTask> {
    return new DataSource({
      load: () => {
        const url = '/incomplete-tasks';
        const params: Record<string, string> = {};

        if (userId) {
          params['executor_id'] = userId;
          params['mentor_id'] = userId;
        }

        return lastValueFrom(
          this.http.get<{ count: number; result: string; tasks: RawTask[] }>(url, { params })
        ).then((data) => {
          if (data?.result === 'ok' && Array.isArray(data.tasks)) {
            return {
              data: data.tasks.map((task) => ({
                ...task,
                executor_ids: task.executors?.map((e: any) => e.user_id) || [],
                mentor_ids: task.mentors?.map((e: any) => e.user_id) || [],
              }) as JBTask),
              totalCount: data.count,
            };
          }

          return {
            data: [],
            totalCount: 0,
          };
        });
      },
      update: (key: any, values: Record<string, any>) => {
        // key 為要更新的資料的 key，values 為 DevExtreme 編輯後的資料
        console.log('Updating task with key', key, 'and values', values);
        return lastValueFrom(this.http.put<ApiResponse<RawTask>>(`/tasks/${key.id}`, {
          ...key,
          ...values
        }))
          .then(res => {
            if (res?.result === 'ok' && res.data) {
              return {
                ...res.data,
              } as JBTask;
            }
            throw new Error(res?.result || 'Update failed');
          });
      },
    });
  }

  displayProgress = (row: JBTask): string => {
    return `${Math.round((row.progress || 0) * 100)}%`;
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