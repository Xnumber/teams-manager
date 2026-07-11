
import { Component, ViewChild } from '@angular/core';
import { DxChartModule, DxDataGridModule, DxSelectBoxModule, DxDataGridComponent } from 'devextreme-angular';
import DataSource, { DataSourceOptions } from 'devextreme/data/data_source';
import { Team, TeamsDataSourceService } from './teams-data-source.service';
import { TeamsLookupDataSourceService } from './lookup-data-source.service';
import { Column, OptionChangedEvent } from 'devextreme/ui/data_grid';
import { BehaviorSubject } from 'rxjs';
import { CommonModule } from '@angular/common';
import query from 'devextreme/data/query';
@Component({
  selector: 'app-teams',
  standalone: true,
  imports: [
    DxDataGridModule,
    DxSelectBoxModule,
    DxChartModule,
    CommonModule
  ],
  templateUrl: './teams.html',
  styleUrl: './teams.scss',
  providers: [TeamsDataSourceService, TeamsLookupDataSourceService]
})
export class Teams {
  teamsDataSource: DataSource<any>;
  tenantsDataSource: DataSourceOptions<any>;
  columns: Column[] = [];
  teamsChartDataSource = new BehaviorSubject<any[]>([]);
  teamsChartDataSource$ = this.teamsChartDataSource.asObservable();
  @ViewChild('dataGrid') dataGrid: DxDataGridComponent | undefined;
  constructor(
    private teamsDataSourceService: TeamsDataSourceService,
    private teamsLookupDataSourceService: TeamsLookupDataSourceService
  ) {
    this.teamsDataSource = this.teamsDataSourceService.createTeamsDataSource();
    this.tenantsDataSource = this.teamsLookupDataSourceService.createTenantLookupDataSource();
    this.columns = [
      { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
      {
        dataField: 'tenant_id',
        caption: '組織',
        validationRules: [{ type: 'required' }],
        lookup: { dataSource: this.tenantsDataSource, valueExpr: 'id', displayExpr: 'name' },
      },
      { dataField: 'tenant_name', caption: '組織名稱', visible: false, formItem: { visible: false } },
      { dataField: 'name', caption: '團隊名稱', validationRules: [{ type: 'required' }] },
      { dataField: 'tasks_count_in_queue', caption: '待辦工作', validationRules: [{ type: 'required' }] },
      { dataField: 'description', caption: '描述' },
      { dataField: 'concurrency_stamp', caption: 'Concurrency Stamp', visible: false, formItem: { visible: false } },
      { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false, formItem: { visible: false } }
    ];


    this.teamsDataSource.store().on('loaded', (data: Team[]) => {
      const taskInQueueResult = query(data as Team[])
        .sortBy('tasks_count_in_queue', false)
        .toArray();
      this.teamsChartDataSource.next(taskInQueueResult);
    })

  }



  
  
  onOptionChanged(e: OptionChangedEvent) {
    const ds = e.component.getDataSource() as DataSource<Team[]>;
    if (
      e.fullName === 'filterValue' ||
      e.fullName.includes('selectedFilterOperation') ||
      e.fullName.includes('filter')
    ) {
      ds.store().load().then((allData) => {
        const filterExpr = this.dataGrid?.instance.getCombinedFilter(true);
        console.log('Filter changed:', filterExpr);
        let q = query(allData as Team[])
        .sortBy('tasks_count_in_queue', false)
        if (filterExpr) {
          q = q
            .filter(filterExpr ?? [])
        }
        const result = q.toArray();
        this.teamsChartDataSource.next(result);
        console.log(result);
      });
    }

    // if (e.fullName.includes('sortOrder')) {
    //   const sorts = ds.sort();
    //   ds.store().load().then((allData) => {
    //     console.log('Sort changed:', sorts);
    //     let q = query(allData as Team[]);

    //     if (Array.isArray(sorts)) {
    //       sorts.forEach((s: any, index) => {
    //         if (index === 0) {
    //           q = q.sortBy(s.selector, s.desc);
    //         } else {
    //           q = q.thenBy(s.selector, s.desc);
    //         }
    //       });
    //     }

    //     const result = q.toArray();
    //     this.teamsChartDataSource.next(result);
    //   });
    // }
  }

  onFilterRowChanged(e: any) {
    console.log('Filter row changed:', e);
  }

  customTooltip(pointInfo: any) {
    return {
      text: `${pointInfo.argumentText} - ${pointInfo.valueText}`
    };
  }
}