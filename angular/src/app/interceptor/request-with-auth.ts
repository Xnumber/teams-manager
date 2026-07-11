
import { inject } from '@angular/core';
import { Auth } from '../services/auth';
import { HttpInterceptorFn } from '@angular/common/http';
import { environment } from '../../enviroments/enviroment';
import { catchError } from 'rxjs/internal/operators/catchError';
const host = environment.apiUrl;

export const RequestWithAuthInterceptor: HttpInterceptorFn = (req, next) => {
  const auth = inject(Auth);
  let headers: Record<string, string> = {};
  if (auth.getToken()) {
    headers['Authorization'] = `Bearer ${auth.getToken()}`;
  }
  if (auth.getTenantId()) {
    headers['X-Tenant-Id'] = auth.getTenantId()!;
  }
  if (auth.getTenantName()) {
    headers['X-Tenant-Name'] = auth.getTenantName()!;
  }
  const cloned = req.clone({
    setHeaders: headers,
  });
  return next(cloned).pipe(catchError((error) => {
    console.error('HTTP error occurred:', error);
    console.error('HTTP error occurred:', error.status);
    if (error.status === 401) {
      console.warn('Unauthorized request. Redirecting to login page.');
    }
    throw error;
  }));
};