import { HttpClient } from '@angular/common/http';
import { Component, effect, inject, input, output, signal } from '@angular/core';
import { DxButtonModule, DxFormModule, DxNumberBoxModule, DxPopupModule } from 'devextreme-angular';



/**
 * Task Editor Component
 * 輸入: visible, task
 */
@Component({
  selector: 'app-task-editor',
  imports: [
    DxFormModule,
    DxPopupModule,
    DxButtonModule,
    DxNumberBoxModule
  ],
  templateUrl: './task-editor.html',
  styleUrl: './task-editor.scss',
})
export class TaskEditor {
  private readonly http = inject(HttpClient);
  visible = input<boolean>(false);
  closed = output<void>();
  task = input<{ id?: number; title?: string; estimatedWorkdays?: number } | null>(null);
  estimatedWorkdays = signal<number | null>(null);

  isSubmitting = signal<boolean>(false);
  errorMessage = signal<string>('');

  constructor() {
    effect(() => {
      this.estimatedWorkdays.set(this.task()?.estimatedWorkdays ?? 0);
    });
  }

  onClose(): void {
    this.errorMessage.set('');
    this.closed.emit();
  }

  onEstimatedWorkdaysChanged(e: { value?: number | null }): void {
    this.estimatedWorkdays.set(e.value ?? 0);
  }


  onRemoveScheduledStartDate(): void {
    console.log('onRemoveScheduledStartDate', this.task());
    const taskId = this.task()?.id;
    if (!taskId || this.isSubmitting()) {
      return;
    }

    this.isSubmitting.set(true);
    this.errorMessage.set('');

    this.http.delete(`/tasks/delete-scheduled-start-date/${taskId}`).subscribe({
      next: () => {
        this.isSubmitting.set(false);
        this.closed.emit();
      },
      error: (err) => {
        this.isSubmitting.set(false);
        this.errorMessage.set(err?.error?.message || '移除預定開始日期失敗');
      }
    });
  }

  onUpdateEstimatedWorkdays(): void {
    const taskId = this.task()?.id;
    const value = this.estimatedWorkdays();

    if (taskId == null || this.isSubmitting()) {
      return;
    }

    if (value == null || value < 0) {
      this.errorMessage.set('請輸入 0 以上的預估工作天數');
      return;
    }

    this.isSubmitting.set(true);
    this.errorMessage.set('');

    this.http.put('/tasks/estimated-workdays', {
      task_id: String(taskId),
      estimated_workdays: value,
    }).subscribe({
      next: () => {
        this.isSubmitting.set(false);
        this.closed.emit();
      },
      error: (err) => {
        this.isSubmitting.set(false);
        this.errorMessage.set(err?.error?.message || '更新預估工作天數失敗');
      }
    });
  }
}