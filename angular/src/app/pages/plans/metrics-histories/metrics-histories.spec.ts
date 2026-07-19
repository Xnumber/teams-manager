import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MetricsHistories } from './metrics-histories';

describe('MetricsHistories', () => {
  let component: MetricsHistories;
  let fixture: ComponentFixture<MetricsHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [MetricsHistories]
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
