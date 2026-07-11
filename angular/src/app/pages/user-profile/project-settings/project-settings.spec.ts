import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ProjectSettings } from './project-settings';

describe('ProjectSettings', () => {
  let component: ProjectSettings;
  let fixture: ComponentFixture<ProjectSettings>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [ProjectSettings]
    })
    .compileComponents();

    fixture = TestBed.createComponent(ProjectSettings);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
