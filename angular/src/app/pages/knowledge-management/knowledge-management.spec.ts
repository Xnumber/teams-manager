import { ComponentFixture, TestBed } from '@angular/core/testing';

import { KnowledgeManagement } from './knowledge-management';

describe('KnowledgeManagement', () => {
  let component: KnowledgeManagement;
  let fixture: ComponentFixture<KnowledgeManagement>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [KnowledgeManagement]
    })
    .compileComponents();

    fixture = TestBed.createComponent(KnowledgeManagement);
    component = fixture.componentInstance;
    await fixture.whenStable();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
