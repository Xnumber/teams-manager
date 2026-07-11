import { Component, EventEmitter, Input, Output, signal } from '@angular/core';
import { finalize } from 'rxjs';
import { Task } from '../../services/task';

@Component({
  selector: 'app-complete-task-button',
  imports: [],
  templateUrl: './complete-task-button.html',
  styleUrl: './complete-task-button.scss',
})
export class CompleteTaskButton {
  @Input() taskId = '';
 
  @Input() completed = false;
  @Output() onCompleted = new EventEmitter<unknown>();
  @Output() failed = new EventEmitter<string>();
  label = signal('完成工作');
  disabled = signal(false);
  isSubmitting = false;
  errorMessage = '';

  constructor(private taskService: Task) {}

  ngOnInit(): void {
    if (this.completed) {
      this.label.set('已完成');
      this.disabled.set(true);
    }
  }

  completeTask(): void {
    if (!this.taskId || this.isSubmitting || this.disabled()) {
      return;
    }

    this.errorMessage = '';
    // this.isSubmitting = true;
    this.label.set('處理中...');
    this.disabled.set(true);
    this.taskService
      .setTaskcompleted(this.taskId)
      .pipe(finalize(() => (this.isSubmitting = false)))
      .subscribe({
        next: (res) => {
          if (res.result === 'ok') {
            this.onCompleted.emit(res.task);
            return;
          }
          const message = res.message || '完成工作失敗';
          this.errorMessage = message;
          this.failed.emit(message);
          this.label.set('已完成');
          this.disabled.set(true);
        },
        error: (err) => {
          const message = err?.error?.message || err?.message || '完成工作失敗';
          this.errorMessage = message;
          this.failed.emit(message);
          this.disabled.set(false);
        },
      });
  }
}