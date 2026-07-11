import { ComponentFixture, TestBed } from '@angular/core/testing';

import { Qwertyuiop2 } from './qwertyuiop2';

describe('Qwertyuiop2', () => {
  let component: Qwertyuiop2;
  let fixture: ComponentFixture<Qwertyuiop2>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [Qwertyuiop2]
    })
    .compileComponents();

    fixture = TestBed.createComponent(Qwertyuiop2);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
