import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TeamProgressCompare } from './team-progress-compare';

describe('TeamProgressCompare', () => {
  let component: TeamProgressCompare;
  let fixture: ComponentFixture<TeamProgressCompare>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [TeamProgressCompare]
    })
    .compileComponents();

    fixture = TestBed.createComponent(TeamProgressCompare);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
