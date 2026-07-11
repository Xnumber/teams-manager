
import { Component } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { TenantsDataSourceService } from './tenants-data-source.service';
import { Tenant } from '../../types/tenant';

@Component({
  selector: 'app-tenants',
  standalone: true,
  imports: [DxDataGridModule],
  templateUrl: './tenants.html',
  styleUrl: './tenants.scss',
  providers: [TenantsDataSourceService]
})
export class Tenants {
  tenantsDataSource: DataSource<Tenant>;
  columns: any[] = [
    { dataField: 'id', caption: 'ID', visible: false, formItem: { visible: false } },
    { dataField: 'name', caption: '名稱', validationRules: [{ type: 'required' }] },
    { dataField: 'description', caption: '描述' },
    { dataField: 'concurrency_stamp', caption: 'Concurrency Stamp', visible: false, formItem: { visible: false } },
    { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', visible: false, formItem: { visible: false } }
  ];

  constructor(private tenantsDataSourceService: TenantsDataSourceService) {
    this.tenantsDataSource = this.tenantsDataSourceService.createTenantsDataSource();
  }
}
