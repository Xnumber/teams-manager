import { Component } from '@angular/core';
import { DxChartModule } from 'devextreme-angular';
import { GrossProduct, Service } from './service.service';

@Component({
  selector: 'app-team-progress-compare',
  imports: [DxChartModule],
  templateUrl: './team-progress-compare.html',
  styleUrl: './team-progress-compare.scss',
  providers: [Service],
})
export class TeamProgressCompare {
  grossProductData: GrossProduct[];

  constructor(service: Service) {
    this.grossProductData = service.getGrossProductData();
  }

  onPointClick(e: any) {
    e.target.select();
  }
}
