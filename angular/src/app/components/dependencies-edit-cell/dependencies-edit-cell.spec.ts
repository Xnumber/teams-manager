import { ComponentFixture, TestBed } from '@angular/core/testing';

import { DependenciesEditCell } from './dependencies-edit-cell';

describe('DependenciesEditCell', () => {
  let component: DependenciesEditCell;
  let fixture: ComponentFixture<DependenciesEditCell>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [DependenciesEditCell]
    })
    .compileComponents();

    fixture = TestBed.createComponent(DependenciesEditCell);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
