import { Component } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular';
import DataSource from 'devextreme/data/data_source';
import { TaskType } from './typing';
import { TaskTypesDataSourceService } from './task-types-data-source.service';

@Component({
  selector: 'app-task-types',
  imports: [DxDataGridModule],
  templateUrl: './task-types.html',
  styleUrl: './task-types.scss',
})
export class TaskTypes {
  taskTypes: DataSource<TaskType>;

  constructor(
    private taskTypesDataSourceService: TaskTypesDataSourceService
  ) {
    this.taskTypes = this.taskTypesDataSourceService.createTaskTypesDataSource();
  }
}
