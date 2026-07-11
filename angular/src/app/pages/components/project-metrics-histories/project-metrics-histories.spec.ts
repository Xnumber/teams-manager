import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ProjectMetricsHistories } from './project-metrics-histories';

describe('ProjectMetricsHistories', () => {
  let component: ProjectMetricsHistories;
  let fixture: ComponentFixture<ProjectMetricsHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [ProjectMetricsHistories]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ProjectMetricsHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
