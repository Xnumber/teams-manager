import { Component, inject, signal } from '@angular/core';
import { TasksDataSourceService } from './tasks-data-source.service';
import { Auth } from '../../../services/auth';
import { DataSource } from 'devextreme/common/data';
import { JBTask } from '../../tasks/typing';
import { DxDataGridModule, DxFormModule, DxSelectBoxModule, DxTabPanelModule } from "devextreme-angular";
import { months, years } from './const';
import { forkJoin, from } from 'rxjs';
import { formatDate } from 'devextreme/localization';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-user-profile-summary',
  templateUrl: './summary.component.html',
  styleUrls: ['./summary.component.scss'],
  providers: [TasksDataSourceService],
  imports: [
    DxSelectBoxModule,
    DxFormModule,
    DxTabPanelModule,
    DxDataGridModule
]
})
export class UserProfileSummaryComponent {

  http = inject(HttpClient);
  weightedMap: Record<number, number> = {
    1: 1,
    2: 1.3,
    3: 1.6,
    4: 1.9,
    5: 2.2,
    6: 2.5,
    7: 2.8,
    8: 3.1,
    9: 3.4,
    10: 3.7
  }


  githubWeightedMap: Record<number, number> = {
    1: 1,
    2: 1.3,
    3: 1.6,
    4: 1.9,
    5: 2.2,
    6: 2.5,
    7: 2.8,
    8: 3.1,
    9: 3.4,
    10: 3.7
  }
  isThisMonthLoaded = signal(true);
  historyChecked = signal(false);
  count = signal(0);
  completedCountThisMonth = signal(0);
  completedCountByMonth = signal(0);

  pRsThisMonth = signal(0);
  commitsThisMonth = signal(0);


  inCompleteCount = signal(0);
  
  scoreByTask = signal(0);

  historyFilterFormData = {
    year: '2026',
    month: '04'
  }

  years = years;

  months = months

  /**
   * 參與的專案數量
   */
  participatingProjectCountThisMonth = signal(0);
  participatingRepositoryCountThisMonth = signal(0);

  participatingProjectCountByMonth = signal(0);

  /**
   * 本月完成的工作參與的專案數量。
   */
  // participatingProjectCountThisMonth = signal(0);

  /**
   * 指定月份完成的工作數量
   */
  completedTasksCountByMonth = signal(0);

  /**
   * 本月完成的工作數量
   */
  completedTasksCountThisMonth = signal(0);

  completedTasksDataSource: DataSource<JBTask> | null = null;
  completedTasksByMonthDataSource: DataSource<JBTask> | null = null;
  completedTasksThisMonthDataSource: DataSource<JBTask> | null = null;
  inCompletedTasksDataSource: DataSource<JBTask> | null = null;
  inCompletedTasksThisMonthDataSource: DataSource<JBTask> | null = null;
  participatingProjectThisMonthDataSource: DataSource<any> | null = null;
  participatingProjectByMonthDataSource: DataSource<any> | null = null;

  /**
   * github
   */
  pRsThisMonthDataSource = signal<any>(null);
  commitsThisMonthDataSource = signal<any>(null);
  workLoadThisMonth = signal(0);
  contributionThisMonth = signal(0);

  /**
   * 
   * @param tasksDataSourceService 
   * @param auth 
   */
  constructor(
    private tasksDataSourceService: TasksDataSourceService,
    private auth: Auth
  ) {
    const userId = this.auth.getUserId();
    if (userId) {

      const month = formatDate(new Date(), 'yyyy-MM')
      this.completedTasksThisMonthDataSource = tasksDataSourceService.createCompletedTasksByMonthDataSource(
        userId,
        month
      )

      this.completedTasksThisMonthDataSource.on('changed', () => {
        const totalCount = this.completedTasksThisMonthDataSource?.totalCount() ?? 0
        this.completedTasksCountThisMonth.set(totalCount)
      })



      this.completedTasksDataSource = tasksDataSourceService.createCompletedTasksDataSource(userId);
      this.completedTasksDataSource.load().then(data => {
        const totalCount = this.completedTasksDataSource?.totalCount() ?? data.length;
        this.count.set(totalCount);
      });
      this.participatingProjectThisMonthDataSource = tasksDataSourceService.createParticipatingProjectThisMonthDataSource(userId);
      this.participatingProjectThisMonthDataSource.load().then(data => {
        const totalCount = this.participatingProjectThisMonthDataSource?.totalCount() ?? data.length;
        this.participatingProjectCountThisMonth.set(totalCount);
      });

      this.inCompletedTasksDataSource = tasksDataSourceService.createInCompletedTasksDataSource(userId);
      this.inCompletedTasksDataSource.load().then(data => {
        const totalCount = this.inCompletedTasksDataSource?.totalCount() ?? data.length;
        this.inCompleteCount.set(totalCount);
      });
      this.inCompletedTasksThisMonthDataSource = tasksDataSourceService.createInCompletedTasksThisMonthDataSource(userId);
      this.inCompletedTasksThisMonthDataSource.load().then(data => {
        const totalCount = this.inCompletedTasksThisMonthDataSource?.totalCount() ?? data.length;
        this.completedCountThisMonth.set(totalCount);
      });
    }

    this.getGitInfoThisMonth();
  }

  gitInfoByMonthDataSource: any = null;
  
  commitCountByMonth = signal(0);
  pRCountByMonth = signal(0);
  githubWorkLoadByMonth = signal(0);
  githubContributionByMonth = signal(0);


  repositoryCountByMonth = signal(0);
  participatingRepositoryCountByMonth = signal(0);

  prsByMonthDataSource = signal<any>(null);
  commitsByMonthDataSource = signal<any>(null);

  /**
   * 歷史查詢
   */
  lookupHistory = () => {
    this.participatingProjectByMonthDataSource = this.tasksDataSourceService.createParticipatingProjectsByMonthDataSource(
      this.auth.getUserId() ?? '',
      `${this.historyFilterFormData.year}-${this.historyFilterFormData.month}`
    );
    this.completedTasksByMonthDataSource = this.tasksDataSourceService.createCompletedTasksByMonthDataSource(
      this.auth.getUserId() ?? '',
      `${this.historyFilterFormData.year}-${this.historyFilterFormData.month}`
    );

    this.gitInfoByMonthDataSource = this.http.get(`/statistic-git-info-by-month?month=${this.historyFilterFormData.year}-${this.historyFilterFormData.month}`);


    forkJoin([
      from(this.participatingProjectByMonthDataSource.load()),
      from(this.completedTasksByMonthDataSource.load()),
      from(this.gitInfoByMonthDataSource)
    ]).subscribe(([projectData, taskData, githubData]: [any, any, any]) => {
      const projectCount = this.participatingProjectByMonthDataSource?.totalCount() ?? projectData.length;
      const taskCount = this.completedTasksByMonthDataSource?.totalCount() ?? taskData.length;
      this.participatingProjectCountByMonth.set(projectCount);
      this.completedTasksCountByMonth.set(taskCount);
      const commit_count  = (githubData as any)['commit_count'] || 0;
      this.commitCountByMonth.set(githubData['commit_count'] || 0);


      const githubContribution = commit_count + (githubData['pr_count'] || 0) * 10;
      const githubWorkLoad = Math.round((((githubData['pr_count'] || 0) + commit_count) * this.githubWeightedMap[githubData['repository_count'] || 0]) * 100) / 100;
      this.prsByMonthDataSource.set(githubData['pull_requests'] || []);
      this.commitsByMonthDataSource.set(githubData['commits'] || []);
      this.pRCountByMonth.set(githubData['pr_count'] || 0);
      this.repositoryCountByMonth.set(githubData['repository_count'] || 0);
      this.githubWorkLoadByMonth.set(githubWorkLoad);
      this.githubContributionByMonth.set(githubContribution);
      this.historyChecked.set(true);
    });
  }


  
  getGitInfoThisMonth() {
    this.http.get<any>('/statistic-git-info-this-month').subscribe(res => {
      console.log('Git info this month:', res);
      this.pRsThisMonth.set(res.pr_count || 0);
      this.commitsThisMonth.set(res.commit_count || 0);
      this.pRsThisMonthDataSource.set(res.pull_requests || []);
      this.commitsThisMonthDataSource.set(res.commits || []);
      this.participatingRepositoryCountThisMonth.set(res.repository_count || 0);
      this.contributionThisMonth.set(res.contribution || 0);
      this.workLoadThisMonth.set(Math.round(((res.work_load || 0) * this.githubWeightedMap[res.repository_count || 0]) * 100) / 100);
    });
  }
}