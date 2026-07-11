import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MilestoneEstimationHistories } from './milestone-estimation-histories';

describe('MilestoneEstimationHistories', () => {
  let component: MilestoneEstimationHistories;
  let fixture: ComponentFixture<MilestoneEstimationHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [MilestoneEstimationHistories]
    })
    .compileComponents();

    fixture = TestBed.createComponent(MilestoneEstimationHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
