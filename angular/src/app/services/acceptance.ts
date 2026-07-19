import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

type CreateAcceptanceRequestResponse = {
  result: string;
  data?: unknown;
  message?: string;
};

@Injectable({
  providedIn: 'root',
})
export class AcceptanceApiService {
  constructor(private http: HttpClient) {}

  createRequest(taskId: string, acceptors: string[]): Observable<CreateAcceptanceRequestResponse> {
    return this.http.post<CreateAcceptanceRequestResponse>('/acceptances', {
      task_id: taskId,
      acceptors,
    });
  }
}