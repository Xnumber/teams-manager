import { TestBed } from '@angular/core/testing';

import { Plans } from './plans';

describe('Plans', () => {
  let service: Plans;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(Plans);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
