import { Component, inject } from '@angular/core';
import { DxDataGridModule } from 'devextreme-angular/ui/data-grid';
import { ProjectsService } from '../../../services/projects';
import { Column, DataErrorOccurredEvent } from 'devextreme/ui/data_grid';

@Component({
  selector: 'app-project-settings',
  imports: [
    DxDataGridModule
  ],
  providers: [
    ProjectsService
  ],
  templateUrl: './project-settings.html',
  styleUrl: './project-settings.scss',
})
export class ProjectSettings {
  projectSettingsDataSource: any;
  projectsService = inject(ProjectsService);
  constructor(
  ) {
    this.projectSettingsDataSource = this.projectsService.createProfileProjectSettingsDataSource();
  }

  
  columns: Column[] = [
    {
      dataField: 'project_id',
      caption: '專案',
      visible: true,
      lookup: {
        dataSource: this.projectsService.createUserProjectsLookupDataSource(),
        displayExpr: 'name',
        valueExpr: 'id'
      },
      formItem: { visible: true },
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'excecutor_time_ratio',
      caption: '平均工時占比',
      dataType: 'number',
      editorOptions: {
        min: 0,
        max: 1,
        step: 0.05,
        format: {
          type: 'percent'
        }
      },
      format: "percent",
      visible: true,
      formItem: { visible: true },
      validationRules: [{ type: 'required' }]
    },
  ]
}