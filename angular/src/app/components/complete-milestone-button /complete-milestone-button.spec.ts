import { ComponentFixture, TestBed } from '@angular/core/testing';
import { provideHttpClient } from '@angular/common/http';
import { provideHttpClientTesting } from '@angular/common/http/testing';

import { CompleteTaskButton } from './complete-milestone-button';

describe('CompleteTaskButton', () => {
  let component: CompleteTaskButton;
  let fixture: ComponentFixture<CompleteTaskButton>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [CompleteTaskButton],
      providers: [provideHttpClient(), provideHttpClientTesting()],
    })
    .compileComponents();

    fixture = TestBed.createComponent(CompleteTaskButton);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
