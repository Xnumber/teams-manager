import { ComponentFixture, TestBed } from '@angular/core/testing';
import { provideHttpClient } from '@angular/common/http';
import { provideHttpClientTesting } from '@angular/common/http/testing';

import { AcceptanceRequestButton } from './acceptance-request-button';

describe('AcceptanceRequestButton', () => {
  let component: AcceptanceRequestButton;
  let fixture: ComponentFixture<AcceptanceRequestButton>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [AcceptanceRequestButton],
      providers: [provideHttpClient(), provideHttpClientTesting()],
    })
    .compileComponents();

    fixture = TestBed.createComponent(AcceptanceRequestButton);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
