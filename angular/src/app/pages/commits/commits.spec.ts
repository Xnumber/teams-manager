import { ComponentFixture, TestBed } from '@angular/core/testing';

import { Commits } from './commits';

describe('Commits', () => {
  let component: Commits;
  let fixture: ComponentFixture<Commits>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [Commits]
    })
    .compileComponents();

    fixture = TestBed.createComponent(Commits);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
