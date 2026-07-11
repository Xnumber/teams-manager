
import { Component, ViewChild } from '@angular/core';
import { DxChartModule, DxDataGridComponent, DxDataGridModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { UsersDataSourceService } from './users-data-source.service';
import { User } from './typing';
import { LookupDataSourceService } from './lookup-data-source.service';
import { Team } from '../team/team';
import query from 'devextreme/data/query';
import { Column, OptionChangedEvent } from 'devextreme/ui/data_grid';
import { BehaviorSubject } from 'rxjs/internal/BehaviorSubject';
import { CommonModule } from '@angular/common';


@Component({
  selector: 'app-users',
  imports: [DxDataGridModule, DxChartModule, CommonModule],
  templateUrl: './users.html',
  styleUrl: './users.scss',
})
export class Users {
  usersDataSource: DataSource<User>;
  teamsDataSource: any;
  tenantDataSource: any;
  columns: Column[] = [];
  @ViewChild('dataGrid') dataGrid: DxDataGridComponent | undefined;
  userTasksChartDataSource: BehaviorSubject<User[]> = new BehaviorSubject<User[]>([]);
  userTasksChartDataSource$ = this.userTasksChartDataSource.asObservable();
  userProjectsChartDataSource: BehaviorSubject<User[]> = new BehaviorSubject<User[]>([]);
  userProjectsChartDataSource$ = this.userProjectsChartDataSource.asObservable();
  constructor(
    private usersDataSourceService: UsersDataSourceService,
    private lookupDataSourceService: LookupDataSourceService
  ) {
    this.tenantDataSource = this.lookupDataSourceService.createTenantDataSource();
    this.teamsDataSource = this.lookupDataSourceService.createTeamDataSource();
    this.usersDataSource = this.usersDataSourceService.createUsersDataSource();

    this.columns = [
      { dataField: 'id', caption: 'ID', visible: false },
      {
        dataField: 'tenant_id',
        caption: '組織',
        lookup: { dataSource: this.tenantDataSource, valueExpr: 'id', displayExpr: 'name' },
        validationRules: [{ type: 'required' }],
      },
      {
        dataField: 'team_id',
        caption: '團隊',
        lookup: { dataSource: this.teamsDataSource, valueExpr: 'id', displayExpr: 'name' },
        validationRules: [{ type: 'required' }],
      },
      { dataField: 'username', caption: '名稱', validationRules: [{ type: 'required' }] },
      // { dataField: 'tenant_name', caption: '', validationRules: [{ type: 'required' }] },
      { dataField: 'email', visible: false, caption: 'Email', validationRules: [{ type: 'required' }, { type: 'email' }] },
      { dataField: 'left_tasks', caption: '剩餘工作', validationRules: [{ type: 'required' }, { type: 'email' }] },
      { dataField: 'project_count', caption: '目前參與專案數', validationRules: [{ type: 'required' }, { type: 'email' }] },
      { dataField: 'password', caption: '密碼', visible: false, formItem: { visible: true, editorType: 'dxTextBox', editorOptions: { mode: 'password' } } },
      { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false },
    ];

    this.usersDataSource.store().on('loaded', (data: User[]) => {
      const userInQueueResult = query(data as User[])
        .sortBy('left_tasks', false)
        .toArray();
      const projectsInQueueResult = query(data as User[])
        .sortBy('project_count', false)
        .toArray();
      this.userTasksChartDataSource.next(userInQueueResult);
      this.userProjectsChartDataSource.next(projectsInQueueResult);
    })
  }



  onOptionChanged(e: OptionChangedEvent) {
    const ds = e.component.getDataSource() as DataSource<User[]>;
    if (
      e.fullName === 'filterValue' ||
      e.fullName.includes('selectedFilterOperation') ||
      e.fullName.includes('filter')
    ) {
      ds.store().load().then((allData) => {
        const filterExpr = this.dataGrid?.instance.getCombinedFilter(true);
        
        let userInQueue = query(allData as User[])
          .sortBy('left_tasks', false)
        
        if (filterExpr) {
          userInQueue = userInQueue
            .filter(filterExpr ?? [])
        }

        const userInQueueResult = userInQueue.toArray();

        let projectsInQueue = query(allData as User[])
          .sortBy('project_count', false)
        
        if (filterExpr) {
          projectsInQueue = projectsInQueue
            .filter(filterExpr ?? [])
        }
        const projectsInQueueResult = projectsInQueue.toArray();



        this.userTasksChartDataSource.next(userInQueueResult);
        this.userProjectsChartDataSource.next(projectsInQueueResult);
      });
    }
  }


  
  customTooltip(pointInfo: any) {
    return {
      text: `${pointInfo.argumentText} - ${pointInfo.valueText}`
    };
  }
}