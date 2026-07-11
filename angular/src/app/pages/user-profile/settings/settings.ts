
import { Component, signal } from '@angular/core';
import { DxButtonModule, DxTextBoxModule } from 'devextreme-angular';
import { HttpClient } from '@angular/common/http';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-settings',
  imports: [DxButtonModule, DxTextBoxModule, CommonModule],
  templateUrl: './settings.html',
  styleUrl: './settings.scss',
})
export class Settings {
  github_login_username = '';
  loading = signal(false);
  result = signal<any>(null);
  error = signal<string | null>(null);

  constructor(private http: HttpClient) {}

  async bindGithubUser() {
    this.loading.set(true);
    this.result.set(null);
    this.error.set(null);
    try {
      const res = await this.http.post<any>('/bind-github-user', { pat: this.github_login_username }).toPromise();
      this.result.set(res);
    } catch (e: any) {
      this.error.set(e?.error?.message || e.message || '綁定失敗');
    } finally {
      this.loading.set(false);
    }
  }
}