
import { CommonModule } from '@angular/common';
import { Component, inject, signal, ViewChild } from '@angular/core';
import { DxButtonModule, DxChartModule, DxDataGridComponent, DxDataGridModule, DxTagBoxModule, DxLoadPanelModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import query from 'devextreme/data/query';
import { ProjectsDataSourceService } from './projects-data-source.service';
import { ProjectsLookupDataSourceService } from './lookup-data-source.service';
import { Project } from '../../types/project';
import { Milestone } from '../../types/milestone';
import { ProjectMilestonesDataSourceService } from './milestones-data-source.service';
import { DxoDataGridRowDraggingComponent } from 'devextreme-angular/ui/data-grid/nested';
import { OptionChangedEvent } from 'devextreme/ui/data_grid';
import { BehaviorSubject, map } from 'rxjs';
import { ProjectEstimationHistories } from '../../components/project-estimation-histories/project-estimation-histories';
import { Team, TeamsDataSourceService } from '../teams/teams-data-source.service';
import { TeamsLookupDataSourceService } from '../teams/lookup-data-source.service';
import { OrganizationOverviewData } from './type';
import { HttpClient } from '@angular/common/http';
import { ProjectMetricsHistories } from '../components/project-metrics-histories/project-metrics-histories';
@Component({
  selector: 'app-projects',
  imports: [
    DxDataGridModule,
    DxButtonModule,
    DxChartModule,
    CommonModule,
    ProjectEstimationHistories,
    DxTagBoxModule,
    DxLoadPanelModule,
    ProjectMetricsHistories
],
  providers: [
    ProjectsDataSourceService, 
    ProjectsLookupDataSourceService,
    TeamsDataSourceService,
    TeamsLookupDataSourceService
  ],
  templateUrl: './projects.html',
  styleUrl: './projects.scss',
})
export class Projects {
  selectedProjectChartTypes = signal<("left" | "delta" | "convergence" | "list")[]>(['convergence']);
  projectChartTypes = [
    { id: 'convergence', name: '預估下次發布日' },
    { id: 'left', name: '剩餘工作大項' },
    { id: 'delta', name: '累積超前/落後工作日' },
    { id: 'list', name: '工作大項列表' },
  ];
  http = inject(HttpClient);
  projectsDataSource: DataSource<Project>;
  tenantLookup: any;
  columns: any[] = [];
  milestoneColumns: any[] = [];
  milestonesLeftProjects = signal<Project[]>([]);
  @ViewChild('dataGrid') dataGrid: DxDataGridComponent | undefined;
  projectsTaskInQueueChartDataSource: BehaviorSubject<Project[]> = new BehaviorSubject<Project[]>([]);
  projectsTaskInQueueChartDataSource$ = this.projectsTaskInQueueChartDataSource.asObservable();
  projectsTaskInQueueCount = signal(0);
  projectsMilestonesInQueueChartDataSource: BehaviorSubject<Project[]> = new BehaviorSubject<Project[]>([]);
  projectsMilestonesInQueueChartDataSource$ = this.projectsMilestonesInQueueChartDataSource.asObservable();
  projectsMilestonesInQueueCount = signal(0);
  teamsDataSource: DataSource<Team>;
  teamsInQueueCount = signal(0);
  teamsChartDataSource = new BehaviorSubject<Team[]>([]);
  teamsChartDataSource$ = this.teamsChartDataSource.asObservable();
  private milestonesDataSourceCache = new Map<string, DataSource<Milestone>>();
  showProjectsGrid = signal(true);
  organizationOverview$ = this.http.get<OrganizationOverviewData>('/organization/overview').pipe(
    map((res: any) => res.data as OrganizationOverviewData)
  );
  loadingEvents = signal<{ event: string }[]>([]);
  constructor(
    private projectsDataSourceService: ProjectsDataSourceService,
    private projectsLookupDataSourceService: ProjectsLookupDataSourceService,
    private projectMilestonesDataSourceService: ProjectMilestonesDataSourceService,
    private teamsDataSourceService: TeamsDataSourceService
  ) {
    this.projectsDataSource = this.projectsDataSourceService.createProjectsDataSource();
    this.teamsDataSource = this.teamsDataSourceService.createTeamsDataSource();
    this.tenantLookup = this.projectsLookupDataSourceService.createTenantLookupDataSource();
    this.columns = [
      { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
      { dataField: 'priority', caption: '優先度', dataType: 'number', formItem: { visible: false } },
      { dataField: 'name', caption: '專案名稱', validationRules: [{ type: 'required' }] },
      { dataField: 'description', caption: '描述' },
      { dataField: 'milestones_count_in_queue', caption: '待辦工作大項', validationRules: [{ type: 'required' }] },
      { dataField: 'tasks_count_in_queue', caption: '待辦工作', validationRules: [{ type: 'required' }] },
      // { dataField: 'test_count', caption: '測試數', dataType: 'number' },
      // { dataField: 'test_pass_count', caption: '通過測試數', dataType: 'number' },
      // { dataField: 'test_auto_count', caption: '自動測試數', dataType: 'number' },
      // { dataField: 'available_developer_count', caption: '可用開發者', dataType: 'number' },
      // { dataField: 'unit_test_coverage', caption: '單元測試覆蓋率', dataType: 'number', format: { type: 'percent', precision: 2 } },
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

    this.projectsDataSource.store().on('loading', (data: Project[]) => {
      this.loadingEvents.set([...this.loadingEvents(), { event: 'load-projects' }]);
    });
    this.projectsDataSource.store().on('loaded', (data: Project[]) => {
        const taskInQueueResult = query(data as Project[])
          .sortBy('tasks_count_in_queue', false)
          .toArray();
        const milestonesInQueueResult = query(data as Project[])
          .sortBy('milestones_count_in_queue', false)
          .toArray();
        this.projectsTaskInQueueChartDataSource.next(taskInQueueResult);
        this.projectsMilestonesInQueueChartDataSource.next(milestonesInQueueResult);
        const todosLeftProjects = data.filter(project => project.tasks_count_in_queue > 0 || project.milestones_count_in_queue > 0);
        this.projectsTaskInQueueCount.set(taskInQueueResult.reduce((sum, project) => sum + (project.tasks_count_in_queue || 0), 0));
        this.projectsMilestonesInQueueCount.set(milestonesInQueueResult.reduce((sum, project) => sum + (project.milestones_count_in_queue || 0), 0));
        if (todosLeftProjects.length > 0) {
          this.milestonesLeftProjects.set(todosLeftProjects);
          console.warn('以下專案有待辦工作或工作大項:', todosLeftProjects);
        }
        this.loadingEvents.set(this.loadingEvents().filter(event => event.event !== 'load-projects'));
    });

    this.teamsDataSource.store().on('loading', (data: Team[]) => {
      this.loadingEvents.set([...this.loadingEvents(), { event: 'load-teams' }]);
    })
    this.teamsDataSource.store().on('loaded', (data: Team[]) => {
      const taskInQueueResult = query(data as Team[])
        .sortBy('tasks_count_in_queue', false)
        .toArray();
      this.teamsChartDataSource.next(taskInQueueResult);
      this.teamsInQueueCount.set(taskInQueueResult.reduce((sum, team) => sum + (team.tasks_count_in_queue || 0), 0));
      this.loadingEvents.set(this.loadingEvents().filter(event => event.event !== 'load-teams'));
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
        
        
        let taskInQueue = query(allData as Project[])
          .sortBy('tasks_count_in_queue', false)
        
        if (filterExpr) {
          taskInQueue = taskInQueue
            .filter(filterExpr ?? [])
        }
        const taskInQueueResult = taskInQueue.toArray();

        let milestonesInQueue = query(allData as Project[])
          .sortBy('milestones_count_in_queue', false)

        if (filterExpr) {
          milestonesInQueue = milestonesInQueue
            .filter(filterExpr ?? [])
        }
        
        const milestonesInQueueResult = milestonesInQueue.toArray();

        this.projectsTaskInQueueChartDataSource.next(taskInQueueResult);
        this.projectsTaskInQueueCount.set(taskInQueueResult.length);
        this.projectsMilestonesInQueueChartDataSource.next(milestonesInQueueResult);
        this.projectsMilestonesInQueueCount.set(milestonesInQueueResult.length);
        
        const todosLeftProjects = milestonesInQueueResult.filter(project => project.tasks_count_in_queue > 0 || project.milestones_count_in_queue > 0);
        
        this.milestonesLeftProjects.set(todosLeftProjects);
      });
    }
  }

  onFilterRowChanged(e: any) {
    console.log('Filter row changed:', e);
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
    const newPriority = event.toIndex + 1;

    this.projectsDataSourceService
      .reprioritizeMilestone(id, newPriority, tenantId)
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
      text: `${pointInfo.argumentText} - ${pointInfo.valueText} 項`
    };
  }

  toggleProjectsGrid() {
    this.showProjectsGrid.update((visible) => !visible);
  }
}