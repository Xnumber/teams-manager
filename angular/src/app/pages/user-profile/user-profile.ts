import { Component, signal } from '@angular/core';
import { DxBoxModule } from 'devextreme-angular';
import { Router, RouterModule } from '@angular/router';
import { Auth } from '../../services/auth';
@Component({
  selector: 'app-user-profile',
  imports: [
    DxBoxModule,
    RouterModule
  ],
  templateUrl: './user-profile.html',
  styleUrl: './user-profile.scss',
})
export class UserProfile {
  userName = signal('John Doe');
  constructor(
    private auth: Auth,
    private router: Router
  ) {}
  ngOnInit() {
    this.auth.isLoggedIn.subscribe(isLoggedIn => {
      if (!isLoggedIn) {
        this.router.navigate(['/login']);
      } else  {
        const userName = this.auth.getUserName();
        if (userName) {
          console.log('User Name:', userName);
          this.userName.set(userName);
        }
      }
    });
  }
}