
import { CommonModule } from '@angular/common';
import { Component, inject, signal, ViewChild } from '@angular/core';
import { DxChartModule, DxDataGridComponent, DxDataGridModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import query from 'devextreme/data/query';
import { ProjectsDataSourceService } from './projects-data-source.service';
import { ProjectsLookupDataSourceService } from './lookup-data-source.service';
import { Project } from '../../types/project';
import { Milestone } from '../../types/milestone';
import { ProjectMilestonesDataSourceService } from './milestones-data-source.service';
import { DxoDataGridRowDraggingComponent } from 'devextreme-angular/ui/data-grid/nested';
import { Column, InitializedEvent, OptionChangedEvent, RowExpandedEvent, RowExpandingEvent } from 'devextreme/ui/data_grid';
import { BehaviorSubject, lastValueFrom } from 'rxjs';
import { DependenciesEditCell } from '../../components/dependencies-edit-cell/dependencies-edit-cell';
import { Task } from '../../services/task';
import { FilterDescriptor } from 'devextreme/data';
import { JBTask } from '../tasks/typing';

@Component({
  selector: 'app-projects',
  imports: [
    DxDataGridModule,
    DxChartModule, 
    CommonModule,
    DependenciesEditCell
  ],
  providers: [
    ProjectsDataSourceService, 
    ProjectsLookupDataSourceService,
  ],
  templateUrl: './projects.html',
  styleUrl: './projects.scss',
})
export class Projects {
  
  taskService = inject(Task);
  projectsDataSource: DataSource<Project>;
  // tasksDataSource = signal<DataSource<JBTask> | null>(null);
  tasksDataSource = this.taskService.createTasksDataSource();
  tenantLookup: any;
  columns: Column[] = [];
  milestoneColumns: Column[] = [];
  taskColumns: Column[] = []
  @ViewChild('dataGrid') dataGrid: DxDataGridComponent | undefined;
  projectsChartDataSource: BehaviorSubject<Project[]> = new BehaviorSubject<Project[]>([]);
  projectsChartDataSource$ = this.projectsChartDataSource.asObservable();
  private milestonesDataSourceCache = new Map<string, DataSource<Milestone>>();

  constructor(
    private projectsDataSourceService: ProjectsDataSourceService,
    private projectsLookupDataSourceService: ProjectsLookupDataSourceService,
    private projectMilestonesDataSourceService: ProjectMilestonesDataSourceService
  ) {
    this.projectsDataSource = this.projectsDataSourceService.createProjectsDataSource();
    this.tenantLookup = this.projectsLookupDataSourceService.createTenantLookupDataSource();
    this.columns = [
      { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
      { dataField: 'priority', caption: '優先度', dataType: 'number', formItem: { visible: false } },
      { dataField: 'name', caption: '專案名稱', validationRules: [{ type: 'required' }] },
      { dataField: 'description', caption: '描述' },
      // { dataField: 'milestones_count_in_queue', caption: '待辦工作大項', validationRules: [{ type: 'required' }] },
      // { dataField: 'tasks_count_in_queue', caption: '待辦工作', validationRules: [{ type: 'required' }] },
      // { dataField: 'test_count', caption: '測試數', dataType: 'number' },
      // { dataField: 'test_pass_count', caption: '通過測試數', dataType: 'number' },
      // { dataField: 'test_auto_count', caption: '自動測試數', dataType: 'number' },
      // { dataField: 'available_developer_count', caption: '可用開發者', dataType: 'number' },
      // { dataField: 'unit_test_coverage', caption: '單元測試覆蓋率', dataType: 'number', format: { type: 'percent', precision: 2 } },
      { dataField: 'dependencies', caption: '依賴', visible: false, editCellTemplate: 'dependenciesEditCellTemplate' },
      { dataField: 'concurrency_stamp', caption: 'Concurrency', visible: false, formItem: { visible: false } },
      { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false, formItem: { visible: false } },
    ];

    this.milestoneColumns = [
      { dataField: 'priority', caption: '優先度' },
      { dataField: 'name', caption: '工作大項名稱' },
      { dataField: 'description', caption: '描述' },
      { dataField: 'team_name', caption: '團隊' },
      { dataField: 'tenant_name', caption: '組織' },
      // { dataField: 'start_date', caption: '開始日期', dataType: 'date' },
      // { dataField: 'end_date', caption: '結束日期', dataType: 'date' },
      // { dataField: 'remark', caption: '備註' },
      // { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false },
    ];













    this.taskColumns = [
      { dataField: 'priority', caption: '優先度', sortOrder: 'asc' },
      { dataField: 'name', caption: '名稱' },
      { dataField: 'description', caption: '描述' },
      { dataField: 'milestone_id', caption: '描述', visible: false, filterValue: '123' },
      // { dataField: 'start_date', caption: '開始日期', dataType: 'date' },
      // { dataField: 'end_date', caption: '結束日期', dataType: 'date' },
      // { dataField: 'remark', caption: '備註' },
      // { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false },
    ];

    this.projectsDataSource.store().on('loaded', (data: Project[]) => {
      this.projectsChartDataSource.next(data);
    });
  }

  onOptionChanged(e: OptionChangedEvent) {
    const ds = e.component.getDataSource() as DataSource<Project[]>;

    if (
      e.fullName === 'filterValue' ||
      e.fullName.includes('selectedFilterOperation') ||
      e.fullName.includes('filter')
    ) {
      ds.store().load().then((allData) => {
        const filterExpr = this.dataGrid?.instance.getCombinedFilter(true);
        const result = query(allData as Project[])
          .filter(filterExpr)
          .toArray();
        this.projectsChartDataSource.next(result);
        console.log('result', result);
      });
    }
    if (e.fullName.includes('sortOrder')) {
      const sorts = ds.sort();
      ds.store().load().then((allData) => {
        let q = query(allData as Project[]);

        if (Array.isArray(sorts)) {
          sorts.forEach((s: any, index) => {
            if (index === 0) {
              q = q.sortBy(s.selector, s.desc);
            } else {
              q = q.thenBy(s.selector, s.desc);
            }
          });
        }

        const result = q.toArray();
        this.projectsChartDataSource.next(result);
      });
    }
  }

  private shouldRefreshChart(fullName?: string): boolean {
    if (!fullName) {
      return false;
    }

    return (
      fullName === 'filterValue' ||
      fullName.includes('selectedFilterOperation') ||
      fullName.includes('filter') ||
      fullName.includes('searchPanel.text') ||
      fullName.includes('sortOrder')
    );
  }

  getMilestonesDataSource(projectId: string): DataSource<Milestone> {
    let dataSource = this.milestonesDataSourceCache.get(projectId);
    if (!dataSource) {
      dataSource = this.projectMilestonesDataSourceService.createMilestonesDataSource(projectId);
      this.milestonesDataSourceCache.set(projectId, dataSource);
    }

    return dataSource;
  }

  onProjectReorder: typeof DxoDataGridRowDraggingComponent.prototype.onReorder = (event) => {
    console.log('Project reordered:', event);

    event.component.beginCustomLoading("正在重新排序...");
    const id = event.itemData.id;
    const newPriority = event.toIndex + 1;
    const tenantId = event.itemData.tenant_id;
    this.projectsDataSourceService
      .reprioritize(id, newPriority, tenantId)
      .then(() => event.component.refresh())
      .then(() => {
        const ds = event.component.getDataSource() as DataSource<Project[]>;
        return ds.reload();
      })
      .catch((error: unknown) => {
        console.error('Project reprioritize failed:', error);
        event.component.refresh();
      })
      .finally(() => {
        event.component.endCustomLoading();
      });
  }

  onMilestoneReorder: typeof DxoDataGridRowDraggingComponent.prototype.onReorder = (event) => {
    console.log('Milestone reordered:', event);

    event.component.beginCustomLoading("正在重新排序...");
    const id = event.itemData.id;
    const tenantId = event.itemData.tenant_id;
    const projectId = event.itemData.project_id;
    const newPriority = event.toIndex;

    this.projectsDataSourceService
      .reprioritizeMilestone(projectId, id, newPriority, tenantId)
      .then(() => event.component.refresh())
      .catch((error: unknown) => {
        console.error('Project reprioritize failed:', error);
        event.component.refresh();
      })
      .finally(() => {
        event.component.endCustomLoading();
      });
  }

  customTooltip(pointInfo: any) {
    return {
      text: `${pointInfo.argumentText} - ${pointInfo.valueText}`
    };
  }

  onMilestoneRowExpanded(event: RowExpandedEvent) {
    const milestoneId = event.key.id;
    this.tasksDataSource.filter(["milestone_id", "=", milestoneId])
  }





  onTasksReorder: typeof DxoDataGridRowDraggingComponent.prototype.onReorder = (event) => {
    event.component.beginCustomLoading("正在重新排序...");
    const id = event.itemData.id;
    const milestoneId = event.itemData.milestone_id;
    const index = event.toIndex;

    this.taskService.reprioritizeTask(id, milestoneId, index + 1)
      .then(() => event.component.refresh())
      .catch((error: unknown) => {
        console.error('Task reprioritize failed:', error);
        event.component.refresh();
      })
      .finally(() => {
        event.component.endCustomLoading();
      });
  }
}