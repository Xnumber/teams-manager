import { Component, inject, signal } from '@angular/core';
import { DxDataGridModule, DxFormComponent, DxFormModule, DxSelectBoxModule, DxTabPanelModule, DxLoadPanelModule, DxNumberBoxModule } from 'devextreme-angular';
import { Data } from './data';
import { ValueChangedEvent } from 'devextreme/ui/select_box';
import { HttpClient } from '@angular/common/http';
import { CommonModule } from '@angular/common';
import { profitWeightMap } from './const';

@Component({
  selector: 'app-reports',
  imports: [
    DxSelectBoxModule,
    DxFormModule,
    DxDataGridModule,
    CommonModule,
    DxTabPanelModule,
    DxLoadPanelModule,
    DxNumberBoxModule
  ],
  templateUrl: './reports.html',
  styleUrl: './reports.scss',
})
export class Reports {
  data = inject(Data);
  teamsDataSource = this.data.createTeamsDataSource();
  teamId: string | null = null;
  month: string | null = null;
  year: string | null = null;

  reports = []
  profit = signal(0);
  teamWorkLoad = signal(0);
  teamContribution = signal(0);

  avgWorkLoad = signal(0);
  avgContribution = signal(0);


  items: DxFormComponent['items'] = [
    {
      dataField: 'teamId',
      label: { text: '團隊' },
      editorType: 'dxSelectBox',
      editorOptions: {
        dataSource: this.teamsDataSource,
        displayExpr: 'name',
        valueExpr: 'id',
        placeholder: 'Select a team',
        onValueChanged: (e: ValueChangedEvent) => {
          this.teamId = e.value;
          console.log('Selected team ID:', this.teamId);
        }
      }
    },
    {
      dataField: 'year',
      label: { text: '年' },
      editorType: 'dxSelectBox',
      editorOptions: {
        dataSource: [
          // { id: '2023', name: '2023' },
          // { id: '2024', name: '2024' },
          // { id: '2025', name: '2025' },
          { id: '2026', name: '2026' },
        ],
        displayExpr: 'name',
        valueExpr: 'id',
        placeholder: '選擇年份',
        onValueChanged: (e: ValueChangedEvent) => {
          this.year = e.value;
        }
      }
    },
    {
      dataField: 'month',
      label: { text: '月' },
      editorType: 'dxSelectBox',
      editorOptions: {
        dataSource: [
          { id: '01', name: '一月' },
          { id: '02', name: '二月' },
          { id: '03', name: '三月' },
          { id: '04', name: '四月' },
          { id: '05', name: '五月' },
          { id: '06', name: '六月' },
          { id: '07', name: '七月' },
          { id: '08', name: '八月' },
          { id: '09', name: '九月' },
          { id: '10', name: '十月' },
          { id: '11', name: '十一月' },
          { id: '12', name: '十二月' }
        ],
        displayExpr: 'name',
        valueExpr: 'id',
        placeholder: '選擇月份',
        onValueChanged: (e: ValueChangedEvent) => {
          this.month = e.value;
          console.log('Selected month:', this.month);
        }
      }
    },
    {
      itemType: 'button',
      horizontalAlignment: 'center',
      buttonOptions: {
        text: '產生報告',
        type: 'success',
        onClick: () => {
          this.generateReport();
          // Here you can add the logic to generate the report based on the selected values
        }
      }
    }
  ]
  statisticsResult = signal([]);
  statisticsError: string | null = null;
  isLoadingStatistics = signal(false);

  constructor(private http: HttpClient) { }
  async generateReport() {
    if (!this.teamId || !this.year || !this.month) {
      alert('請選擇團隊、年份與月份');
      this.statisticsError = '請選擇團隊、年份與月份';
      this.statisticsResult.set([]);
      return;
    }
    this.statisticsError = null;
    this.statisticsResult.set([]);
    this.isLoadingStatistics.set(true);
    try {
      const monthStr = `${this.year}-${this.month}`;
      const params = { team_id: this.teamId, month: monthStr };
      // const data = await this.http.get<any>('/task-statistics', { params }).toPromise();
      const data = await this.http.get<any>('/statistic-by-month', { params }).toPromise();
      if (data.result === 'ok') {
        this.statisticsResult.set(data.data || []);
        this.teamWorkLoad.set(data.team_workload || 0);
        this.teamContribution.set(data.team_contribution || 0);
        this.avgWorkLoad.set(data.team_workload / (data.data.length || 1) || 0);
        this.avgContribution.set(data.team_contribution / (data.data.length || 1) || 0);
      } else {
        this.statisticsError = data.message || '取得統計資料失敗';
      }
    } catch (e: any) {
      this.statisticsError = e?.message || '取得統計資料失敗';
    } finally {
      this.isLoadingStatistics.set(false);
    }
  }






  /**
  * 根據盈餘值取得對應係數 (盈餘單位：萬)
  * @param profit 盈餘金額，單位：萬
  * @returns 對應的係數，如果超出範圍預設回傳 1.0
  */
  getProfitWeightByProfit(profit: number): number {
    for (const range of profitWeightMap) {
      if (profit >= range.min && profit <= range.max) {
        return range.weight;
      }
    }
    return 1.0; // 預設值
  }
  /**
   * 分配係數
   */
  distributeCoefficient = 0.45
  /**
   * 係數中間值
   */
  coefficientMidpoint = 1.025
  /**
   * 工作繁重度係數
   * 最大1.25, 最小0.8
   * 計算方式: 係數中間值+分配係數*平均負載相比
   */
  getWorkLoadCoefficient(avgWorkLoadCompare: number): number {
    const coefficient = this.coefficientMidpoint + this.distributeCoefficient * avgWorkLoadCompare;
    return Math.max(0.8, Math.min(1.25, coefficient));
  }

  /**
   * 績效係數
   * 最大1.25, 最小0.8
   * 計算方式: 係數中間值+分配係數*平均貢獻相比
   */
  getPerformanceCoefficient(avgContributionCompare: number): number {
    const coefficient = this.coefficientMidpoint + this.distributeCoefficient * avgContributionCompare;
    return Math.max(0.8, Math.min(1.25, coefficient));
  }
}