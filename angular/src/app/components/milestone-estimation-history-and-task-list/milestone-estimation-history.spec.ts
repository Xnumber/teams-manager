import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MilestoneEstimationHistoryAndTaskList } from './milestone-estimation-history';

describe('MilestoneEstimationHistoryAndTaskList', () => {
  let component: MilestoneEstimationHistoryAndTaskList;
  let fixture: ComponentFixture<MilestoneEstimationHistoryAndTaskList>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [MilestoneEstimationHistoryAndTaskList]
    })
    .compileComponents();

    fixture = TestBed.createComponent(MilestoneEstimationHistoryAndTaskList);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
