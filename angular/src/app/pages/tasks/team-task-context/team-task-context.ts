import { Component, Input, Output, EventEmitter } from '@angular/core';
import { DxPopupModule, DxFormModule } from 'devextreme-angular';
import { JBTask } from '../typing';
import { LookupDataSourceService } from '../lookup-data-source.service';
import { Item } from 'devextreme/ui/form';

@Component({
  selector: 'app-team-task-context',
  standalone: true,
  imports: [DxPopupModule, DxFormModule],
  templateUrl: './team-task-context.html',
  styleUrl: './team-task-context.scss',
  providers: [LookupDataSourceService]
})
export class TeamTaskContext {
  @Input() visible = false;
  @Input() taskData: Partial<JBTask> = {};
  @Output() closed = new EventEmitter<void>();
  executors: any;
  creators: any;
  mentors: any;
  projects: any;
  taskTypes: any;
  formItems: Item[] = [];
  constructor(
    private lookupDataSourceService: LookupDataSourceService
  ) {
    this.projects = this.lookupDataSourceService.createProjectDataSource();
    this.taskTypes = this.lookupDataSourceService.createTaskTypeDataSource();
    this.executors = this.lookupDataSourceService.createUserDataSource();
    this.creators = this.lookupDataSourceService.createUserDataSource();
    this.mentors = this.lookupDataSourceService.createUserDataSource();
    this.formItems = [
      {
        dataField: 'project_id',
        label: { text: '專案' },
        editorType: 'dxSelectBox',
        colSpan: 2,
        editorOptions: {
          dataSource: this.projects,
          valueExpr: 'id',
          displayExpr: 'name',
          disabled: true,
        },
      },
      {
        dataField: 'name',
        label: { text: '名稱' },
        colSpan: 2,
        isRequired: true,
        editorOptions: { disabled: true },
      },
      {
        dataField: 'task_type_id',
        colSpan: 2,
        label: { text: '任務類型' },
        editorType: 'dxSelectBox',
        editorOptions: {
          dataSource: this.taskTypes,
          valueExpr: 'id',
          displayExpr: 'name',
          disabled: true,
        },
      },
      {
        dataField: 'description',
        label: { text: '描述' },
        editorType: 'dxTextArea',
        colSpan: 6,
        editorOptions: { disabled: true },
      },
      {
        dataField: 'status',
        label: { text: '狀態' },
        editorOptions: { disabled: true },
      },
      {
        dataField: 'progress',
        label: { text: '進度' },
        editorType: 'dxNumberBox',
        editorOptions: {
          min: 0,
          max: 100,
          format: 'percent',
          step: 1,
          disabled: true,
        },
      },
      {
        dataField: 'executor_ids',
        label: { text: '執行者' },
        editorType: 'dxTagBox',
        editorOptions: {
          dataSource: this.executors,
          valueExpr: 'id',
          displayExpr: 'name',
          showSelectionControls: true,
          applyValueMode: 'useButtons',
          disabled: true,
        },
        colSpan: 2,
      },
      {
        dataField: 'mentor_ids',
        label: { text: '指導者' },
        editorType: 'dxTagBox',
        editorOptions: {
          dataSource: this.mentors,
          valueExpr: 'id',
          displayExpr: 'name',
          showSelectionControls: true,
          applyValueMode: 'useButtons',
          disabled: true,
        },
        colSpan: 2,
      },
      {
        dataField: 'scheduled_start_date',
        label: { text: '開始日期' },
        editorType: 'dxDateBox',
        editorOptions: { disabled: true },
      },
      {
        dataField: 'scheduled_end_date',
        label: { text: '結束日期' },
        editorType: 'dxDateBox',
        editorOptions: { disabled: true },
      },
      {
        dataField: 'scheduled_completion_date',
        label: { text: '預定完成日期' },
        editorType: 'dxDateBox',
        editorOptions: { disabled: true },
      },
      {
        dataField: 'completion_date',
        label: { text: '完成日期' },
        editorType: 'dxDateBox',
        editorOptions: { disabled: true },
      },
      {
        dataField: 'remark',
        label: { text: '備註' },
        editorType: 'dxTextArea',
        colSpan: 2,
        editorOptions: { disabled: true },
      },
      {
        itemType: 'empty',
        colSpan: 5,
      },
      {
        itemType: 'group',
        colSpan: 1,
        colCount: 2,
        items: [
          {
            itemType: 'button',
            buttonOptions: {
              type: 'success',
              text: '支援',
              onClick: () => this.onClose(),
            },
            colSpan: 1,
            label: { text: '備註' },
            editorType: 'dxTextArea',
            editorOptions: { disabled: true },
          },
          {
            itemType: 'button',
            buttonOptions: {
              type: 'default',
              text: '執行',
              onClick: () => this.onClose(),
            },
            colSpan: 1,
            label: { text: '備註' },
            editorType: 'dxTextArea',
            editorOptions: { disabled: true },
          },
        ]
      },
    ];
  }

  onClose() {
    this.closed.emit();
  }
}
