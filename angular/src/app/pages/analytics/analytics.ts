
import { Component, inject } from '@angular/core';
import { DxButtonModule, DxChartModule, DxDataGridModule, DxSelectBoxModule, DxTagBoxModule, DxTextBoxModule } from 'devextreme-angular';
import { DxiChartSeriesComponent } from 'devextreme-angular/ui/chart/nested';
import { AnalyticsLookupDataSourceService } from './lookup-data-source.service';
import { HttpClient } from '@angular/common/http';
import { BehaviorSubject } from 'rxjs';
import { CommonModule } from '@angular/common';
import { ProjectEstimationHistories } from '../../components/project-estimation-histories/project-estimation-histories';



@Component({
  selector: 'app-analytics',
  imports: [
    DxDataGridModule,
    DxChartModule,
    DxTagBoxModule,
    DxButtonModule,
    DxSelectBoxModule,
    DxTextBoxModule,
    CommonModule,
    ProjectEstimationHistories
  ],
  templateUrl: './analytics.html',
  styleUrl: './analytics.scss',
})
export class Analytics {
  options = ['選項1', '選項2', '選項3'];
  combinations = ['組合A', '組合B', '組合C'];
  milestoneLookupDataSource: any;

  convergenceData = [
    { x: '1月', a: 80, b: 40, c: 10 },
    { x: '2月', a: 70, b: 45, c: 20 },
    { x: '3月', a: 60, b: 50, c: 30 },
    { x: '4月', a: 55, b: 52, c: 35 },
    { x: '5月', a: 50, b: 53, c: 40 },
    { x: '6月', a: 48, b: 54, c: 45 },
    { x: '7月', a: 47, b: 54, c: 48 },
    { x: '8月', a: 46, b: 54, c: 50 },
    { x: '9月', a: 45, b: 54, c: 52 },
    { x: '10月', a: 30, b: 30, c: 30 },
    { x: '11月', a: 30, b: 30, c: 30 },
    { x: '12月', a: 30, b: 30, c: 30 }
  ];
  todoBurndown = [
    { day: '3/1', value: 30 },
    { day: '3/2', value: 27 },
    { day: '3/3', value: 23 },
    { day: '3/4', value: 20 },
    { day: '3/5', value: 15 },
    { day: '3/6', value: 10 },
    { day: '3/7', value: 5 },
    { day: '3/8', value: 0 },
  ];
  estimateBurndown = [
    { day: '3/1', value: 28 },
    { day: '3/2', value: 25 },
    { day: '3/3', value: 22 },
    { day: '3/4', value: 19 },
    { day: '3/5', value: 15 },
    { day: '3/6', value: 10 },
    { day: '3/7', value: 5 },
    { day: '3/8', value: 0 },
  ];
  actualBurndown = [
    { day: '3/1', value: 32 },
    { day: '3/2', value: 29 },
    { day: '3/3', value: 25 },
    { day: '3/4', value: 22 },
    { day: '3/5', value: 18 },
    { day: '3/6', value: 12 },
    { day: '3/7', value: 7 },
    { day: '3/8', value: 2 },
  ];

  compareBurndown: { day: string; predicted?: number; actual?: number }[] = [
    { day: '3/1', predicted: 10, actual: 11 },
    { day: '3/2', predicted: 9, actual: 10 },
    { day: '3/3', predicted: 7, actual: 9 },
    { day: '3/4', predicted: 6, actual: 8 },
    { day: '3/5', predicted: 5, actual: 7 },
    { day: '3/6', predicted: 4, actual: 6 },
    { day: '3/7', predicted: 3, actual: 3 },
    { day: '3/8', predicted: 2, actual: 1 }
  ];

  progressCompare = [
    { day: '3/1', progress: 0 },
    { day: '3/2', progress: 10 },
    { day: '3/3', progress: 10 },
    { day: '3/4', progress: 5 },
    { day: '3/5', progress: 15 },
    { day: '3/6', progress: -10 },
    { day: '3/7', progress: -5 },
    { day: '3/8', progress: -5 }
  ]

  /**
   * 將 compareBurndown 資料分成兩組：
   * 一組 actual - predicted > 0
   * 一組 actual - predicted < 0
   */
  splitCompareBurndown = () => {
    const greater: typeof this.compareBurndown = [];
    const less: typeof this.compareBurndown = [];
    const result: Record<string, any>[] = [];
    for (const item of this.compareBurndown) {
      if (item.actual === undefined || item.predicted === undefined) {
        less.push({ day: item.day });
        greater.push({ day: item.day });
        continue;
      }

      const diff = item.actual - item.predicted;
      if (diff > 0) {
        console.log('>', diff);
        result.push({ greater: diff, day: item.day });
        // less.push({ day: item.day,actual: 0, predicted: 0 }); // 將 actual 設為 predicted，讓它在圖表上與 predicted 重疊
      } else if (diff < 0) {
        console.log('<', diff);
        result.push({ lesser: diff, day: item.day });
        // greater.push({ day: item.day, actual: 0, predicted: 0 }); // 將 actual 設為 predicted，讓它在圖表上與 predicted 重疊
      } else {
        console.log('=', diff);
        result.push({ day: item.day });
      }
    }


    console.log('result', result);
    return result;
  }
  data: any[] = []
  series: DxiChartSeriesComponent[] = []
  http = inject(HttpClient);
  constructor(
    private analyticsLookupDataSourceService: AnalyticsLookupDataSourceService,
  ) {
    this.data = this.splitCompareBurndown()
  };
  data2 = [
    { x: 'Jan', y: 30 }, { x: 'Feb', y: 55 }, { x: 'Mar', y: 20 },
    { x: 'Apr', y: -10 }, { x: 'May', y: -35 }, { x: 'Jun', y: -15 },
    { x: 'Jul', y: 10 }, { x: 'Aug', y: 45 }, { x: 'Sep', y: 60 },
    { x: 'Oct', y: 25 }, { x: 'Nov', y: -20 }, { x: 'Dec', y: 15 }
  ];

  milestoneId = '';
  mileStoneConvergenceData = [
    { x: '1月', a: 80, b: 40, c: 10 },
  ];
  private milestoneId$ = new BehaviorSubject<string | null>(null);
  private reload$ = new BehaviorSubject<void>(void 0);
}