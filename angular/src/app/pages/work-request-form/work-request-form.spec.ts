import { ComponentFixture, TestBed } from '@angular/core/testing';

import { WorkRequestForm } from './work-request-form';

describe('WorkRequestForm', () => {
  let component: WorkRequestForm;
  let fixture: ComponentFixture<WorkRequestForm>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [WorkRequestForm]
    })
    .compileComponents();

    fixture = TestBed.createComponent(WorkRequestForm);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
