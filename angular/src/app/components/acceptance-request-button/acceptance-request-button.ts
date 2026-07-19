import { Component, EventEmitter, Input, Output, inject, signal } from '@angular/core';
import { DxButtonModule, DxPopupModule, DxTagBoxModule } from 'devextreme-angular';
import { finalize } from 'rxjs';
import { AcceptanceApiService } from '../../services/acceptance';
import { UserService } from '../../services/user';

@Component({
  selector: 'app-acceptance-request-button',
  standalone: true,
  imports: [DxButtonModule, DxPopupModule, DxTagBoxModule],
  templateUrl: './acceptance-request-button.html',
  styleUrl: './acceptance-request-button.scss',
})
export class AcceptanceRequestButton {
  private acceptanceService = inject(AcceptanceApiService);
  private userService = inject(UserService);

  @Input() taskId = '';
  @Output() requested = new EventEmitter<unknown>();
  @Output() failed = new EventEmitter<string>();

  popupVisible = signal(false);
  selectedAcceptorIds = signal<string[]>([]);
  errorMessage = signal('');
  isSubmitting = signal(false);
  acceptors;

  constructor() {
    this.acceptors = this.userService.createUserDataSource();
  }

  openPopup(): void {
    if (!this.taskId) {
      return;
    }

    this.errorMessage.set('');
    this.popupVisible.set(true);
  }

  closePopup(): void {
    this.popupVisible.set(false);
    this.errorMessage.set('');
  }

  onAcceptorChanged(event: { value?: string[] }): void {
    this.selectedAcceptorIds.set(event.value ?? []);
  }

  submit(): void {
    if (!this.taskId || this.isSubmitting() || this.selectedAcceptorIds().length === 0) {
      return;
    }

    this.errorMessage.set('');
    this.isSubmitting.set(true);

    this.acceptanceService
      .createRequest(this.taskId, this.selectedAcceptorIds())
      .pipe(finalize(() => this.isSubmitting.set(false)))
      .subscribe({
        next: (res) => {
          if (res.result === 'ok') {
            this.requested.emit(res.data);
            this.selectedAcceptorIds.set([]);
            this.closePopup();
            return;
          }

          const message = res.message || '送出確認請求失敗';
          this.errorMessage.set(message);
          this.failed.emit(message);
        },
        error: (err) => {
          const message = err?.error?.message || err?.message || '送出確認請求失敗';
          this.errorMessage.set(message);
          this.failed.emit(message);
        },
      });
  }

}
