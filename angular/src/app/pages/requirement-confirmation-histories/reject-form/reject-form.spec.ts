import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RejectForm } from './reject-form';

describe('RejectForm', () => {
  let component: RejectForm;
  let fixture: ComponentFixture<RejectForm>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [RejectForm]
    })
    .compileComponents();

    fixture = TestBed.createComponent(RejectForm);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
