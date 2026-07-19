import { ComponentFixture, TestBed } from '@angular/core/testing';

import { PlanTaskMetricsHistories } from './plan-task-metrics-histories';
import { PlanService } from '../service/plans';

describe('PlanTaskMetricsHistories', () => {
  let component: PlanTaskMetricsHistories;
  let fixture: ComponentFixture<PlanTaskMetricsHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [PlanTaskMetricsHistories],
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

    fixture = TestBed.createComponent(PlanTaskMetricsHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
