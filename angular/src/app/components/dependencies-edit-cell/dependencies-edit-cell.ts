import { Component, inject, input, output, signal, effect } from '@angular/core';
import type { WorkItemDependency, WorkItemType } from './typing';
import { DependencyType } from './typing';
import { DxSelectBoxModule, DxTagBoxModule } from "devextreme-angular";
import { ProjectsService } from '../../services/projects';
import { MilestonesService } from '../../services/milestones';
import { Task } from '../../services/task';
import { ValueChangedEvent } from 'devextreme/ui/tag_box';

import { ValueChangedEvent as SelectValueChangedEvent } from 'devextreme/ui/select_box';

/**
 * 依賴項選擇的數據結構
 */
interface DependencySelection {
  /** 選中的專案 ID 列表 */
  projects: string[];
  /** 選中的大項 ID 列表 */
  milestones: string[];
  /** 選中的工作 ID 列表 */
  tasks: string[];
}


/**
 * 工作項依賴關係編輯組件
 * 用於編輯和管理工作項的依賴項，支持選擇專案、大項和工作作為前置項
 */
@Component({
  selector: 'app-dependencies-edit-cell',
  imports: [DxSelectBoxModule],
  templateUrl: './dependencies-edit-cell.html',
  styleUrl: './dependencies-edit-cell.scss',
})
export class DependenciesEditCell {
  selfId = input<string>();
  selfDependencyType = input<WorkItemType>();
  /** 專案服務 */
  projects = inject(ProjectsService);
  /** 專案數據源 */
  projectsDataSource = this.projects.createProjectLookupOnlyDataSource();
  /** 大項服務 */
  milestones = inject(MilestonesService);
  /** 大項數據源 */
  milestonesDataSource = this.milestones.createMilestoneLookupDataSource();
  /** 工作服務 */
  tasks = inject(Task);
  /** 工作數據源 */
  tasksDataSource = this.tasks.createTasksLookupDataSource();
  /** 初始依賴項列表 */
  value = input<WorkItemDependency[]>()

  /** 初始依賴項列表的副本 */
  _value = signal<WorkItemDependency[]>([]);
  /** 依賴項變化輸出事件 */
  valueChanged = output<WorkItemDependency[]>()

  /** 當前選中的依賴項列表 */
  dependencies = signal<WorkItemDependency[]>([]);

  /** 當前的選擇狀態 */
  selection = signal<DependencySelection>({
    projects: [],
    milestones: [],
    tasks: [],
  });

  constructor() {
    effect(() => {
      const inputValue = this.value();
      if (inputValue) {
        this._value.set([...inputValue]);
      }
    });

    // effect(() => {
    //   this.valueChanged.emit(this.dependencies());
    // });
  }

  /**
   * 專案選擇變化時的處理方法
   * @param event 專案選擇事件
   */
  onProjectsChanged(event: ValueChangedEvent) {
    const newSelection = { ...this.selection(), projects: event.value || [] };
    this.selection.set(newSelection);
    this.updateDependencies();
  }

  /**
   * 大項選擇變化時的處理方法
   * @param event 大項選擇事件
   */
  onMilestonesChanged(event: ValueChangedEvent) {
    const newSelection = { ...this.selection(), milestones: event.value || [] };
    this.selection.set(newSelection);
    this.updateDependencies();
  }

  /**
   * 工作選擇變化時的處理方法
   * @param event 工作選擇事件
   */
  onTasksChanged(event: SelectValueChangedEvent) {
    const newSelection = { ...this.selection(), tasks: [event.value]};
    this.selection.set(newSelection);
    this.updateDependencies();
  }

  /**
   * 根據當前選擇更新依賴項列表
   * 從選擇的專案、大項和工作創建對應的 WorkItemDependency 對象
   */
  private updateDependencies() {
    const newDependencies: WorkItemDependency[] = [];
    const selection = this.selection();
    const currentValue = this._value();
    
    // const successorInfo = {
    //   id: this.selfId(),
    //   type: this.selfDependencyType(), // 假設當前編輯的工作項類型為 TASK
    // }

    // console.log('Current selection:', selection);
    // console.log('Current successorInfo:', successorInfo);

    
    // const successorType = successorInfo.type;
    // const successorId = successorInfo.id;
    
    // if (!successorType) return;
    // if (!successorId) return;

    selection.projects.forEach(projectId => {
      newDependencies.push({
        predecessorType: 'PROJECT',
        predecessorId: projectId,
        // successorType,
        // successorId,
        dependencyType: DependencyType.FS,
        // createdAt: new Date().toISOString(),
      });
    });

    selection.milestones.forEach(milestoneId => {
      newDependencies.push({
        predecessorType: 'MILESTONE',
        predecessorId: milestoneId,
        // successorType,
        // successorId,
        dependencyType: DependencyType.FS,
        // createdAt: new Date().toISOString(),
      });
    });

    selection.tasks.forEach(taskId => {
      newDependencies.push({
        predecessorType: 'TASK',
        predecessorId: taskId,
        // successorType,
        // successorId,
        dependencyType: DependencyType.FS,
        // createdAt: new Date().toISOString(),
      });
    });

    this.dependencies.set(newDependencies);
    this.valueChanged.emit(newDependencies);
    console.log('Updated dependencies:', newDependencies);
  }
}