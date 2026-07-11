import { Component } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular';
import DataSource, { DataSourceOptions } from 'devextreme/data/data_source';
import { RepositoriesDataSourceService } from './repositories-data-source.service';
import { Column } from 'devextreme/ui/data_grid';

@Component({
  selector: 'app-repositories',
  standalone: true,
  imports: [DxDataGridModule],
  templateUrl: './repositories.html',
  styleUrl: './repositories.scss',
  providers: [RepositoriesDataSourceService]
})
export class Repositories {
  repositoriesDataSource: DataSource<any>;
  columns: Column[] = [];
  constructor(
    private repositoriesDataSourceService: RepositoriesDataSourceService
  ) {
    this.repositoriesDataSource = this.repositoriesDataSourceService.createRepositoriesDataSource();
    this.columns = [
      // { dataField: 'id', caption: 'UUID', formItem: { visible: false }, validationRules: [{ type: 'required' }] },
      // { dataField: 'repo_id', caption: 'Repo ID', dataType: 'number', validationRules: [{ type: 'required' }] },
      { dataField: 'owner', caption: 'Owner', validationRules: [{ type: 'required' }] },
      { dataField: 'repo_name', caption: 'Repository Name', validationRules: [{ type: 'required' }] },
      { dataField: 'github_pat', visible: false, caption: 'GitHub PAT', validationRules: [{ type: 'required' }], formItem: { colSpan: 2} },
      // { dataField: 'created_at', caption: 'Created At', dataType: 'datetime', visible: false, formItem: { visible: false } }
    ];
  }
}
