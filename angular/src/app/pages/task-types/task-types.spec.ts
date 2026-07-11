import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TaskTypes } from './task-types';

describe('TaskTypes', () => {
  let component: TaskTypes;
  let fixture: ComponentFixture<TaskTypes>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [TaskTypes]
    })
    .compileComponents();

    fixture = TestBed.createComponent(TaskTypes);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
