import { Component } from '@angular/core';
import { DxChartModule } from 'devextreme-angular';
import { GrossProduct, Service } from './service.service';

@Component({
  selector: 'app-member-progress-compare',
  imports: [DxChartModule],
  templateUrl: './member-progress-compare.html',
  styleUrls: ['./member-progress-compare.scss'],
  providers: [Service],
})
export class MemberProgressCompare {
  grossProductData: GrossProduct[];

  constructor(service: Service) {
    this.grossProductData = service.getGrossProductData();
  }

  onPointClick(e: any) {
    e.target.select();
  }
}
