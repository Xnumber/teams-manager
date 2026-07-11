import { HttpInterceptorFn } from '@angular/common/http';
import { inject } from '@angular/core';
import { Router } from '@angular/router';
import { catchError } from 'rxjs/operators';
import { throwError } from 'rxjs';
import { environment } from '../../enviroments/enviroment';

const host = environment.apiUrl;

export const RequestWithHostInterceptor: HttpInterceptorFn = (req, next) => {
  const router = inject(Router);
  
  console.log('[Function Interceptor] Request URL:', req.url);

  const cloned = req.clone({
    url: /^https?:\/\//.test(req.url) ? req.url : host + req.url,
    setHeaders: { 'X-From-Function': 'true' }
  });

  return next(cloned).pipe(
    catchError(error => {
      if (error.status === 401) {
        console.warn('[Interceptor] 401 Unauthorized - Redirecting to login');
        router.navigate(['/login']);
      }
      return throwError(() => error);
    })
  );
};