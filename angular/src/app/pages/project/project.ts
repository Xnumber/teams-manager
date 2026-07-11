import { Component } from '@angular/core';
import { DxChartModule } from 'devextreme-angular';

@Component({
  selector: 'app-project',
  imports: [DxChartModule],
  templateUrl: './project.html',
  styleUrl: './project.scss',
})
export class Project {
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
}
