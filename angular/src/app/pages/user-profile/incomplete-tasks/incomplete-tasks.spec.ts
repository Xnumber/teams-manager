import { ComponentFixture, TestBed } from '@angular/core/testing';

import { IncompleteTasks } from './incomplete-tasks';

describe('IncompleteTasks', () => {
  let component: IncompleteTasks;
  let fixture: ComponentFixture<IncompleteTasks>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [IncompleteTasks]
    })
    .compileComponents();

    fixture = TestBed.createComponent(IncompleteTasks);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
