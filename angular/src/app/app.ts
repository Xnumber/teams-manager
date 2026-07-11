import { Component, signal } from '@angular/core';
import { RouterLink, RouterLinkActive, RouterOutlet } from '@angular/router';
import { routes } from './app.routes';
import DxForm, { Properties } from "devextreme/ui/form";
import DxChart, { Properties as ChartProperties } from "devextreme/viz/chart";
import { Auth } from './services/auth';

DxForm.defaultOptions<Properties>({
  options: {
    labelLocation: 'top',
  }
});

DxChart.defaultOptions<ChartProperties>({
  options: {
    title: {
      horizontalAlignment: 'left',
      font: {
        size: 30,
        weight: 600
      }
    }
  }
});

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, RouterLink, RouterLinkActive],
  templateUrl: './app.html',
  styleUrl: './app.scss'
})
export class App {
  constructor(
    protected auth: Auth
  ) { }

  isLoggedIn = signal(false);

  username = signal('');
  ngOnInit() {

    this.auth.isLoggedIn.subscribe(isLoggedIn => {
      this.isLoggedIn.set(isLoggedIn);
      const username = this.auth.getUserName();
      this.username.set(username ? username : '');
    });
    this.auth.initializeAuthInfo();
  }

  protected readonly title = signal('angular');

  // 導覽列資料
  protected readonly routes = routes;
}