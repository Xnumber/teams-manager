import { Injectable } from '@angular/core';
import { BehaviorSubject, Subject } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class Auth {
  private token: string | null = null;
  private tenant_id: string | null = null;
  private tenant_name: string | null = null;
  private user_id: string | null = null;
  private team_id: string | null = null;
  private team_name: string | null = null;
  private user_name: string | null = null;
  public isLoggedIn: BehaviorSubject<boolean> = new BehaviorSubject<boolean>(false);
  constructor() {
    // 從 localStorage 初始化
    this.initializeAuthInfo();
  }

  initializeAuthInfo = () => {
    this.token = localStorage.getItem('token');
    this.tenant_id = localStorage.getItem('tenant_id');
    this.tenant_name = localStorage.getItem('tenant_name');
    this.user_id = localStorage.getItem('user_id');
    this.user_name = localStorage.getItem('user_name');
    this.team_id = localStorage.getItem('team_id');
    this.team_name = localStorage.getItem('team_name');
    this.isLoggedIn.next(!!this.token);
  }

  setAuthInfo(info: { token: string; user_id: string; tenant_id: string; tenant_name: string, team_id: string; team_name: string, user_name: string }) {
    this.token = info.token;
    this.tenant_id = info.tenant_id;
    this.tenant_name = info.tenant_name;
    this.user_id = info.user_id;
    this.user_name = info.user_name;
    this.team_id = info.team_id;
    this.team_name = info.team_name;

    // 存到 localStorage
    localStorage.setItem('token', info.token);
    localStorage.setItem('tenant_id', info.tenant_id);
    localStorage.setItem('user_id', info.user_id);
    localStorage.setItem('user_name', info.user_name);
    localStorage.setItem('tenant_name', info.tenant_name);
    localStorage.setItem('team_id', info.team_id);
    localStorage.setItem('team_name', info.team_name);
    this.isLoggedIn.next(true);
  }

  getToken(): string | null {
    return this.token;
  }

  getTenantId(): string | null {
    return this.tenant_id;
  }

  getTenantName(): string | null {
    return this.tenant_name;
  }

  getUserId(): string | null {
    return this.user_id;
  }

  getUserName(): string | null {
    return this.user_name;
  }




  
  getTeamId(): string | null {
    return this.team_id;
  }

  clear() {
    this.token = null;
    this.tenant_id = null;
    this.tenant_name = null;
    this.user_id = null;
    this.user_name = null;
    this.team_id = null;
    this.team_name = null;
    
    // 清除 localStorage
    localStorage.removeItem('token');
    localStorage.removeItem('tenant_id');
    localStorage.removeItem('tenant_name');
    localStorage.removeItem('user_id');
    localStorage.removeItem('team_id');
    localStorage.removeItem('team_name');
  }
}
