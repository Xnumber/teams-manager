import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RequirementConfirmationRequests } from './requirement-confirmation-requests';

describe('RequirementConfirmationRequests', () => {
  let component: RequirementConfirmationRequests;
  let fixture: ComponentFixture<RequirementConfirmationRequests>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [RequirementConfirmationRequests]
    })
    .compileComponents();

    fixture = TestBed.createComponent(RequirementConfirmationRequests);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
