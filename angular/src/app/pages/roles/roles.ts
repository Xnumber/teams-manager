import { Component } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { RolesDataSourceService, Role } from './roles-data-source.service';

@Component({
  selector: 'app-roles',
  imports: [DxDataGridModule],
  providers: [RolesDataSourceService],
  templateUrl: './roles.html',
  styleUrl: './roles.scss',
})
export class Roles {
  rolesDataSource: DataSource<Role>;
  columns: any[] = [];

  constructor(private rolesDataSourceService: RolesDataSourceService) {
    this.rolesDataSource = this.rolesDataSourceService.createRolesDataSource();
    this.columns = [
      { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
      { dataField: 'name', caption: '角色名稱', validationRules: [{ type: 'required' }] },
      { dataField: 'description', caption: '描述' },
    ];
  }
}
