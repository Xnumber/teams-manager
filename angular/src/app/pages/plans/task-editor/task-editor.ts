import { HttpClient } from '@angular/common/http';
import { Component, inject, input, output, signal } from '@angular/core';
import { DxButtonModule, DxFormModule, DxPopupModule } from 'devextreme-angular';
import { Task } from '../service/data';



/**
 * Task Editor Component
 * 輸入: visible, task
 */
@Component({
  selector: 'app-task-editor',
  imports: [
    DxFormModule,
    DxPopupModule,
    DxButtonModule
  ],
  templateUrl: './task-editor.html',
  styleUrl: './task-editor.scss',
})
export class TaskEditor {
  private readonly http = inject(HttpClient);

  visible = input<boolean>(false);
  closed = output<void>();
  task = input<Task | null>(null);

  isSubmitting = signal<boolean>(false);
  errorMessage = signal<string>('');

  onClose(): void {
    this.errorMessage.set('');
    this.closed.emit();
  }

  onRemoveScheduledStartDate(): void {
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
}