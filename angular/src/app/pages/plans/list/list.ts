import { Component } from '@angular/core';
import { CommonModule } from '@angular/common';
import { DxDataGridModule } from 'devextreme-angular';
import { Column } from 'devextreme/ui/data_grid';
import { PlanService } from '../service/plans';
import { ProjectsService } from '../../../services/projects';
import { inject } from '@angular/core';

@Component({
  selector: 'app-list',
  standalone: true,
  imports: [CommonModule, DxDataGridModule],
  templateUrl: './list.html',
  styleUrl: './list.scss',
  providers: [PlanService]
})
export class List {
  planService = inject(PlanService);
  projectService = inject(ProjectsService);

  plansDataSource = this.planService.createPlansDataSource();
  projects = this.projectService.createProjectLookupDataSource();

  planColumns: Column[] = [
    {
      dataField: 'id',
      caption: 'ID',
      width: 100,
      allowEditing: false,
      visible: false,
      formItem: { visible: false }
    },
    {
      dataField: 'name',
      caption: '計畫名稱',
      width: 150,
      validationRules: [{ type: 'required' }],
      formItem: { colSpan: 2 }
    },
    {
      dataField: 'start_date',
      caption: '開始日期',
      dataType: 'date',
      width: 120,
      format: 'yyyy-MM-dd',
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'end_date',
      caption: '結束日期',
      dataType: 'date',
      width: 120,
      format: 'yyyy-MM-dd',
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'project_ids',
      caption: '專案',
      width: 150,
      lookup: {
        dataSource: this.projects,
        valueExpr: 'id',
        displayExpr: 'name'
      },
      validationRules: [{ type: 'required' }],
      formItem: { colSpan: 2, editorType: 'dxTagBox' }
    },
    {
      dataField: 'description',
      caption: '說明',
      formItem: { editorType: 'dxTextArea', colSpan: 2 },
      validationRules: [{ type: 'required' }]
    },
    {
      dataField: 'created_at',
      caption: '建立時間',
      dataType: 'datetime',
      width: 140,
      allowEditing: false,
      format: 'yyyy-MM-dd HH:mm:ss',
      visible: false,
      formItem: { visible: false }
    },
    {
      dataField: 'status',
      caption: '狀態',
      width: 100,
      lookup: {
        dataSource: [
          { id: 'draft', text: '草稿' },
          { id: 'active', text: '進行中' },
          { id: 'completed', text: '已完成' },
          { id: 'cancelled', text: '已取消' }
        ],
        valueExpr: 'id',
        displayExpr: 'text'
      },
      validationRules: [{ type: 'required' }]
    }
  ];

}
