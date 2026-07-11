import { Component, EventEmitter, Input, Output, signal } from '@angular/core';
import { finalize } from 'rxjs';
import { Task } from '../../services/task';
import { MilestoneService } from '../../services/milestone';

@Component({
  selector: 'app-complete-milestone-button',
  imports: [],
  templateUrl: './complete-milestone-button.html',
  styleUrl: './complete-milestone-button.scss',
})
export class CompleteMilestoneButton {
  @Input() milestoneId = '';
  @Input() completed = false;
  @Output() onCompleted = new EventEmitter<unknown>();
  @Output() failed = new EventEmitter<string>();
  label = signal('完成');
  disabled = signal(false);
  isSubmitting = false;
  errorMessage = '';

  constructor(private milestoneService: MilestoneService) {}

  ngOnInit(): void {
    if (this.completed) {
      this.label.set('已完成');
      this.disabled.set(true);
    }
  }

  completeMilestone(): void {
    if (!this.milestoneId || this.isSubmitting || this.disabled()) {
      return;
    }

    this.errorMessage = '';
    // this.isSubmitting = true;
    this.label.set('處理中...');
    this.disabled.set(true);
    this.milestoneService
      .setMilestoneCompleted(this.milestoneId)
      .pipe(finalize(() => (this.isSubmitting = false)))
      .subscribe({
        next: (res) => {
          if (res.result === 'ok') {
            this.onCompleted.emit(res.milestone);
            return;
          }
          const message = res.message || '完成里程碑失敗';
          this.errorMessage = message;
          this.failed.emit(message);
          this.label.set('已完成');
          this.disabled.set(true);
        },
        error: (err) => {
          const message = err?.error?.message || err?.message || '完成里程碑失敗';
          this.label.set('完成');
          this.errorMessage = message;
          this.failed.emit(message);
          this.disabled.set(false);
        },
      });
  }
}