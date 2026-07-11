import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ProjectEstimationHistories } from './project-estimation-histories';

describe('ProjectEstimationHistories', () => {
  let component: ProjectEstimationHistories;
  let fixture: ComponentFixture<ProjectEstimationHistories>;
  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [ProjectEstimationHistories]
    })
    .compileComponents();
    fixture = TestBed.createComponent(ProjectEstimationHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });
  it('should create', () => {
    expect(component).toBeTruthy();
  });
});