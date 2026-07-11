import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RequestConfirmationButton } from './request-confirmation-button';

describe('RequestConfirmationButton', () => {
  let component: RequestConfirmationButton;
  let fixture: ComponentFixture<RequestConfirmationButton>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [RequestConfirmationButton]
    })
    .compileComponents();

    fixture = TestBed.createComponent(RequestConfirmationButton);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
