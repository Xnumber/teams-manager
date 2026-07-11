import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

interface SetMilestoneCompletedResponse {
  result: string;
  milestone?: unknown;
  message?: string;
}

@Injectable({
  providedIn: 'root',
})
export class MilestoneService {
  constructor(private http: HttpClient) {}

  setMilestoneCompleted(milestoneId: string): Observable<SetMilestoneCompletedResponse> {
    return this.http.put<SetMilestoneCompletedResponse>(`/milestones/complete/${milestoneId}`, {});
  }
}
