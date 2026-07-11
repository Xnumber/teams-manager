import {  expect } from '@playwright/test';
import { userDataForRegister, tenantBaseData } from './data';
import { test } from '../fixture/fixture';

const TENANT_API = 'http://localhost:5001/tenant';
const REGISTER_API = 'http://localhost:5001/register';

test.describe.serial('身分驗證流程', () => {
  let tenant_desc: string;
  let tenant_id: string;
  test('API 註冊 tenant', async ({ apiContext }) => {
    tenant_desc = tenantBaseData.description;
    const tenantRes = await apiContext.post(TENANT_API, {
      data: {
        name: tenantBaseData.name,
        description: tenant_desc,
      },
      headers: { 'Content-Type': 'application/json' },
    });
    expect(tenantRes.status()).toBe(200);
    const tenantBody = await tenantRes.json();
    tenant_id = tenantBody.tenant.id;
    expect(tenantBody.tenant.name).toBe(tenantBaseData.name);
    expect(tenantBody.tenant.description).toBe(tenant_desc);
  });
  test('API 註冊 user', async ({ apiContext, teams }) => {
    // 用上個測試建立的 tenant 註冊 user
    const res = await apiContext.post(REGISTER_API, {
      data: {
        ...userDataForRegister,
        team_id: teams[0].id, // 假設 teams fixture 已經建立了一個 team，並且我們使用它的 id
      },
    });
    // console.log('註冊 user API 回應:', await res.text());
    expect(res.ok()).toBeTruthy();
    const json = await res.json();
    expect(json).toHaveProperty('data');
    expect(json.data).toHaveProperty('id');
    expect(json.data.username).toBe(userDataForRegister.username);
    expect(json.data.email).toBe(userDataForRegister.email);
  });
  
  test('API 登入 user', async ({ apiContext }) => {
    // 使用前面註冊的 user 資料登入
    const LOGIN_API = 'http://localhost:5001/login';
    const res = await apiContext.post(LOGIN_API, {
      data: {
        tenant_name: userDataForRegister.tenant_name,
        username: userDataForRegister.username,
        password: userDataForRegister.password,
      },
    });

    // console.log('登入 API 回應:', await res.text());
    expect(res.ok()).toBeTruthy();
    const json = await res.json();
    expect(json).toHaveProperty('token');
    expect(json).toHaveProperty('tenant_id', tenant_id);
    expect(json).toHaveProperty('tenant_name', userDataForRegister.tenant_name);
    expect(json).toHaveProperty('message', 'Login success');
  });

  test('API 登入輸錯密碼', async ({ apiContext }) => {
    // 使用錯誤的密碼登入
    const LOGIN_API = 'http://localhost:5001/login';
    const res = await apiContext.post(LOGIN_API, {
      data: {
        tenant_name: userDataForRegister.tenant_name,
        username: userDataForRegister.username,
        password: 'wrongpassword123',
      },
    });
    // expect(res.ok()).toBeTruthy();
    const json = await res.json();
    expect(res.status()).toBe(400);
    expect(json).toMatch(/Wrong password/);
  });





//   test('API 註冊缺少欄位', async ({ request }) => {
//     // 先建立 tenant
//     const tenant_name = tenantMissingData.name + '-' + Date.now();
//     const tenant_desc = tenantMissingData.description;
//     const tenantRes = await request.post(TENANT_API, {
//       data: {
//         name: tenant_name,
//         description: tenant_desc,
//       },
//       headers: { 'Content-Type': 'application/json' },
//     });
//     expect(tenantRes.status()).toBe(200);

//     // 缺少欄位測試
//     const res = await request.post(REGISTER_API, {
//       data: {
//         ...registerMissingFieldsData,
//         tenant_name,
//       },
//     });
//     expect(res.status()).toBe(400);
//   });
});