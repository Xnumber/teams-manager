import { ComponentFixture, TestBed } from '@angular/core/testing';

import { PlanMilestoneMetricsHistories } from './plan-milestone-metrics-histories';
import { PlanService } from '../service/plans';

describe('PlanMilestoneMetricsHistories', () => {
  let component: PlanMilestoneMetricsHistories;
  let fixture: ComponentFixture<PlanMilestoneMetricsHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [PlanMilestoneMetricsHistories],
      providers: [
        {
          provide: PlanService,
          useValue: {
            updateMetricsHistory: async () => ({}),
            listMetricsHistories: async () => [],
          },
        },
      ],
    })
    .compileComponents();

    fixture = TestBed.createComponent(PlanMilestoneMetricsHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
