import { ComponentFixture, TestBed } from '@angular/core/testing';

import { PersonalTasks } from './personal-tasks';

describe('PersonalTasks', () => {
  let component: PersonalTasks;
  let fixture: ComponentFixture<PersonalTasks>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [PersonalTasks]
    })
    .compileComponents();

    fixture = TestBed.createComponent(PersonalTasks);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
