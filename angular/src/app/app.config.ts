import { ApplicationConfig, provideBrowserGlobalErrorListeners } from '@angular/core';
import { provideRouter } from '@angular/router';

import { routes } from './app.routes';
import { provideHttpClient, withInterceptors, withInterceptorsFromDi } from '@angular/common/http';
import { RequestWithHostInterceptor } from './interceptor/request-with-host';
import { RequestWithAuthInterceptor } from './interceptor/request-with-auth';





export const appConfig: ApplicationConfig = {
  providers: [
    provideBrowserGlobalErrorListeners(),
    provideRouter(routes),
    provideHttpClient(withInterceptors([
      RequestWithHostInterceptor,
      RequestWithAuthInterceptor
    ])),
  ]
};