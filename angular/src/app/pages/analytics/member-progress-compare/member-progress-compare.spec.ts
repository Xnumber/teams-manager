import { ComponentFixture, TestBed } from '@angular/core/testing';

import { MemberProgressCompare } from './member-progress-compare';

describe('MemberProgressCompare', () => {
  let component: MemberProgressCompare;
  let fixture: ComponentFixture<MemberProgressCompare>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [MemberProgressCompare]
    })
    .compileComponents();

    fixture = TestBed.createComponent(MemberProgressCompare);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
