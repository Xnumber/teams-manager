import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MetricsHistories } from './metrics-histories';
import { PlanService } from '../service/plans';

describe('MetricsHistories', () => {
  let component: MetricsHistories;
  let fixture: ComponentFixture<MetricsHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [MetricsHistories],
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

    fixture = TestBed.createComponent(MetricsHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
