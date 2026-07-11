import { ComponentFixture, TestBed } from '@angular/core/testing';

import { TeamTaskContext } from './team-task-context';

describe('TeamTaskContext', () => {
  let component: TeamTaskContext;
  let fixture: ComponentFixture<TeamTaskContext>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [TeamTaskContext]
    })
    .compileComponents();

    fixture = TestBed.createComponent(TeamTaskContext);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
