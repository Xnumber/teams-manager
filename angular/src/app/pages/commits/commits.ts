import { Component, signal, inject, viewChild } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { DxButtonModule } from 'devextreme-angular/ui/button';
import { DxDataGridModule } from 'devextreme-angular/ui/data-grid';
import { DxFormComponent, DxFormModule } from 'devextreme-angular';
import { CommonModule } from '@angular/common';
import { Column } from 'devextreme/ui/data_grid';
import { FieldDataChangedEvent } from 'devextreme/ui/form_types';
import { Data } from './data';
import { ValidationCallbackData } from 'devextreme-angular/common';

@Component({
  selector: 'app-commits',
  imports: [DxDataGridModule, DxButtonModule, DxFormModule, CommonModule],
  templateUrl: './commits.html',
  styleUrl: './commits.scss',
})
export class Commits {
  service = inject(Data);
  form = viewChild<DxFormComponent>('form');
  private readonly previousMonthRange = this.getPreviousMonthRange();
  formData: any = {
    repo_id: '',
    github_token: '',
    since: this.previousMonthRange.since,
    until: this.previousMonthRange.until,
    sha: 'develop',
  };
  commitsDataSource: DataSource | null = null;
  repos = this.service.getRepositoriesDataSource();

  columns: Column[] = [
    { dataField: 'commit_sha', caption: 'SHA', width: 100, cellTemplate: 'shaCell' },
    { dataField: 'message', caption: '訊息' },
    { dataField: 'github_author_login', caption: 'Github作者', width: 140 },
    { dataField: 'user_name', caption: '作者', width: 120 },
    { dataField: 'committed_at', caption: '提交時間', dataType: 'datetime', width: 150 },
    { dataField: 'github_repository_name', caption: 'Repository', width: 160 },
    { dataField: 'html_url', caption: '連結', cellTemplate: 'linkCell', width: 70 },
  ];

  loading = signal(false);
  result = signal<any>(null);
  error = signal<string | null>(null);

  private getPreviousMonthRange() {
    const now = new Date();
    const since = new Date(now.getFullYear(), now.getMonth() - 1, 1, 0, 0, 0, 0);
    const until = new Date(now.getFullYear(), now.getMonth(), 0, 23, 59, 59, 999);
    return { since, until };
  }

  async sync() {
    this.loading.set(true);
    this.result.set(null);
    this.error.set(null);

    const validateResult = this.form()?.instance.validate();
    if (validateResult && !validateResult.isValid) {
      const validationMessages = (validateResult.brokenRules ?? [])
        .map(rule => rule.message)
        .filter((message, index, arr) => !!message && arr.indexOf(message) === index);

      this.loading.set(false);
      return;
    }

    try {
      const params: any = {
        repo_id: this.formData.repo_id,
        github_token: this.formData.github_token,
        sha: this.formData.sha || 'develop',
      };
      if (this.formData.since) params['since'] = new Date(this.formData.since).toISOString();
      if (this.formData.until) params['until'] = new Date(this.formData.until).toISOString();

      const res = await this.service.syncCommits(params);
      delete res.skipped_items
      this.result.set(res);
      this.commitsDataSource?.reload();
    } catch (e: any) {
      this.error.set(e?.error?.message || e.message || 'Sync failed');
    } finally {
      this.loading.set(false);
    }
  }
  onFieldDataChanged(e: FieldDataChangedEvent) {
    if (e.dataField === 'repo_id') {
      this.commitsDataSource = this.service.createCommitsDataSource(this.formData.repo_id);
    }
  }



  syncUsers() {
    this.service.syncCommitsAndUsers().then(res => {
      console.log('Sync Commits and Users result:', res);
      // Handle result if needed



      this.commitsDataSource?.reload();
    }).catch(err => {
      console.error('Error syncing Commits and Users:', err);
      // Handle error if needed
    });
    // Implement user sync logic if needed
  }

  sinceValidationCallback = (e: ValidationCallbackData) => {
    const since = new Date(e.value);
    const until = new Date(this.formData.until);
    return !this.formData.until || since < until;
  }

  untilValidationCallback = (e: ValidationCallbackData) => {
    const until = new Date(e.value);
    const since = new Date(this.formData.since);
    return !this.formData.since || until > since;
  }
}