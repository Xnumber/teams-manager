import { Component, signal } from '@angular/core';
import DataSource from 'devextreme/data/data_source';
import { HttpClient } from '@angular/common/http';
import { lastValueFrom } from 'rxjs';
import { DxButtonModule } from 'devextreme-angular/ui/button';
import { DxDataGridModule } from 'devextreme-angular/ui/data-grid';
import { DxDateBoxModule } from 'devextreme-angular/ui/date-box';
import { DxTextBoxModule } from 'devextreme-angular/ui/text-box';
import { inject } from '@angular/core';
import { Data } from './data';
import { DxFormModule } from 'devextreme-angular';
import { CommonModule } from '@angular/common';
import { Column } from 'devextreme/ui/data_grid';
import { FieldDataChangedEvent } from 'devextreme/ui/form_types';
@Component({
  selector: 'app-prs',
  imports: [
    DxDataGridModule,
    DxButtonModule,
    DxDateBoxModule,
    DxTextBoxModule,
    DxFormModule,
    CommonModule,
  ],
  templateUrl: './prs.html',
  styleUrl: './prs.scss',
})
export class Prs {
  service = inject(Data);
  
// JBHRIS/jbjob-cms-angular
  formData = {
    repo_id: '',
    github_token: '',
  };

  pullRequestsDataSource: DataSource | null = this.service.createPullRequestDataSource(this.formData.repo_id);
  columns: Column[] = [
    { dataField: 'pr_number', caption: 'PR編號' },
    { dataField: 'title', caption: '標題' },
    { dataField: 'pr_user_login', caption: 'Github 名稱' },
    { dataField: 'user_name', caption: '名稱' },
    { dataField: 'state', caption: '狀態', width: 80 },
    { dataField: 'is_merged', caption: '已合併', dataType: 'boolean', width: 80 },
    { dataField: 'created_at', caption: '建立時間', dataType: 'datetime', width: 150 },
    { dataField: 'updated_at', caption: '更新時間', dataType: 'datetime', width: 150 },
    { dataField: 'html_url', caption: '連結', cellTemplate: 'linkCell', width: 80 },
  ];
  loading = signal(false);
  result = signal<any>(null);
  error = signal<string | null>(null);
  repos = this.service.getRepositoriesDataSource();
  async sync() {
    this.loading.set(true);
    this.result.set(null);
    this.error.set(null);
    
    
    if (!this.formData.repo_id) {
      alert('請選擇一個 repository');
      this.error.set('請選擇一個 repository');
      this.loading.set(false);
      return;
    }

    if (!this.formData.github_token) {
      alert('請輸入 Github PAT, 僅需Read權限。');
      this.error.set('請輸入 Github Token');
      this.loading.set(false);
      return;
    }

    
    try {

      const res = await this.service.syncPrs(
        {
          repo_id: this.formData.repo_id,
          github_token: this.formData.github_token
        },
        this.formData['github_token']
      );
      this.result.set(res);
      this.pullRequestsDataSource?.reload();
    } catch (e: any) {
      this.error.set(e?.error?.message || e.message || 'Sync failed');
    } finally {
      this.loading.set(false);
    }
  }
  


  onRepoChange(e: any) {
    this.pullRequestsDataSource = this.service.createPullRequestDataSource(this.formData.repo_id);
  }







  onFieldDataChanged(e: FieldDataChangedEvent) {
    console.log('Field data changed:', e);
    if (e.dataField === 'repo_id') {
      this.onRepoChange(e);
    }
  }

  syncUsers() {
    this.service.syncPrsAndUsers().then(res => {
      console.log('Sync PRs and Users result:', res);
      // Handle result if needed



      this.pullRequestsDataSource?.reload();
    }).catch(err => {
      console.error('Error syncing PRs and Users:', err);
      // Handle error if needed
    });
    // Implement user sync logic if needed
  }
}