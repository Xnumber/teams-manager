import { ComponentFixture, TestBed } from '@angular/core/testing';

import { RequirementConfirmationHistories } from './requirement-confirmation-histories';

describe('RequirementConfirmationHistories', () => {
  let component: RequirementConfirmationHistories;
  let fixture: ComponentFixture<RequirementConfirmationHistories>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [RequirementConfirmationHistories]
    })
    .compileComponents();

    fixture = TestBed.createComponent(RequirementConfirmationHistories);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
