import { ComponentFixture, TestBed } from '@angular/core/testing';
import { provideHttpClient } from '@angular/common/http';
import { provideHttpClientTesting } from '@angular/common/http/testing';

import { TaskEditor } from './task-editor';

describe('TaskEditor', () => {
  let component: TaskEditor;
  let fixture: ComponentFixture<TaskEditor>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [TaskEditor],
      providers: [
        provideHttpClient(),
        provideHttpClientTesting()
      ]
    })
    .compileComponents();

    fixture = TestBed.createComponent(TaskEditor);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
