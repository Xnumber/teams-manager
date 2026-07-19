import { ComponentFixture, TestBed } from '@angular/core/testing';

import { EstimationHistories } from './estimation-histories';
import { PlanService } from '../service/plans';

describe('EstimationHistories', () => {
  let component: EstimationHistories;
  let fixture: ComponentFixture<EstimationHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [EstimationHistories],
      providers: [
        {
          provide: PlanService,
          useValue: {
            getPlanHistory: async () => [],
          },
        },
      ],
    })
    .compileComponents();

    fixture = TestBed.createComponent(EstimationHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
