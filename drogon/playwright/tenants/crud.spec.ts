import { test, expect, request } from '@playwright/test';

test('Tenant CRUD 全流程', async () => {
  const apiContext = await request.newContext();

  // 建立租戶
  const tenant_name_send = 'TenantTest';
  const tenant_desc_send = 'TenantTestDesc';
  const createRes = await apiContext.post('http://localhost:5001/tenant', {
    data: {
      name: tenant_name_send,
      description: tenant_desc_send,
    },
    headers: { 'Content-Type': 'application/json' },
  });
  expect(createRes.status(), { message: `Expected status 200 for tenant creation, but got ${createRes.status()}` }).toBe(200);
  const createBody = await createRes.json();
  const tenant_id = createBody.tenant.id;
  expect(createBody.tenant.name, { message: `Expected tenant name to be ${tenant_name_send}, but got ${createBody.tenant.name}` }).toBe(tenant_name_send);
  expect(createBody.tenant.description, { message: `Expected tenant description to be ${tenant_desc_send}, but got ${createBody.tenant.description}` }).toBe(tenant_desc_send);

  // 查詢單一租戶
  const getRes = await apiContext.get(`http://localhost:5001/tenant/${tenant_id}`, {
    headers: { 'Content-Type': 'application/json' },
  });
  expect(getRes.status(), { message: `Expected status 200 for tenant get, but got ${getRes.status()}` }).toBe(200);
  const getBody = await getRes.json();
  expect(getBody.result, { message: `Expected result to be ok, but got ${getBody.result}` }).toBe('ok');
  expect(getBody.tenant.id, { message: `Expected tenant id to be ${tenant_id}, but got ${getBody.tenant.id}` }).toBe(tenant_id);
  expect(getBody.tenant.name, { message: `Expected tenant name to be ${tenant_name_send}, but got ${getBody.tenant.name}` }).toBe(tenant_name_send);
  expect(getBody.tenant.description, { message: `Expected tenant description to be ${tenant_desc_send}, but got ${getBody.tenant.description}` }).toBe(tenant_desc_send);

  // 查詢租戶列表並過濾
  const listRes = await apiContext.get('http://localhost:5001/tenant', {
    headers: { 'Content-Type': 'application/json' },
  });
  expect(listRes.status(), { message: `Expected status 200 for tenant list, but got ${listRes.status()}` }).toBe(200);
  const listBody = await listRes.json();
  expect(listBody.result, { message: `Expected result to be ok, but got ${listBody.result}` }).toBe('ok');
  expect(Array.isArray(listBody.tenants), { message: 'Expected tenants to be an array' }).toBe(true);
  expect(typeof listBody.count, { message: 'Expected count to be a number' }).toBe('number');
  const filtered = listBody.tenants.filter(
    t => t.id === tenant_id && t.name === tenant_name_send && t.description === tenant_desc_send
  );
  expect(filtered.length, { message: `Expected filtered tenants to include created tenant, but got ${filtered.length}` }).toBeGreaterThan(0);

  // 更新租戶
  const tenant_name_update = 'TenantUpdated';
  const tenant_desc_update = 'TenantUpdatedDesc';
  const updateRes = await apiContext.put(`http://localhost:5001/tenant/${tenant_id}`, {
    data: {
      name: tenant_name_update,
      description: tenant_desc_update,
    },
    headers: { 'Content-Type': 'application/json' },
  });
  expect(updateRes.status(), { message: `Expected status 200 for tenant update, but got ${updateRes.status()}` }).toBe(200);
  const updateBody = await updateRes.json();
  expect(updateBody.tenant.id, { message: `Expected tenant id to be ${tenant_id}, but got ${updateBody.tenant.id}` }).toBe(tenant_id);
  expect(updateBody.tenant.name, { message: `Expected tenant name to be ${tenant_name_update}, but got ${updateBody.tenant.name}` }).toBe(tenant_name_update);
  expect(updateBody.tenant.description, { message: `Expected tenant description to be ${tenant_desc_update}, but got ${updateBody.tenant.description}` }).toBe(tenant_desc_update);

  // 更新後再次查詢
  const getRes2 = await apiContext.get(`http://localhost:5001/tenant/${tenant_id}`, {
    headers: { 'Content-Type': 'application/json' },
  });
  expect(getRes2.status(), { message: `Expected status 200 for tenant get after update, but got ${getRes2.status()}` }).toBe(200);
  const getBody2 = await getRes2.json();
  expect(getBody2.result, { message: `Expected result to be ok, but got ${getBody2.result}` }).toBe('ok');
  expect(getBody2.tenant.id, { message: `Expected tenant id to be ${tenant_id}, but got ${getBody2.tenant.id}` }).toBe(tenant_id);
  expect(getBody2.tenant.name, { message: `Expected tenant name to be ${tenant_name_update}, but got ${getBody2.tenant.name}` }).toBe(tenant_name_update);
  expect(getBody2.tenant.description, { message: `Expected tenant description to be ${tenant_desc_update}, but got ${getBody2.tenant.description}` }).toBe(tenant_desc_update);

  
  // 刪除租戶
  // const deleteRes = await apiContext.delete(`http://localhost:5001/tenant/${tenant_id}`, {
  //   headers: { 'Content-Type': 'application/json' },
  // });
  // expect(deleteRes.status(), { message: `Expected status 200 for tenant delete, but got ${deleteRes.status()}` }).toBe(200);
  // const deleteBody = await deleteRes.json();
  // expect(deleteBody.tenant.id, { message: `Expected tenant id to be ${tenant_id}, but got ${deleteBody.tenant.id}` }).toBe(tenant_id);


  


  // 刪除後再次查詢
  // const getRes3 = await apiContext.get(`http://localhost:5001/tenant/${tenant_id}`, {
  //   headers: { 'Content-Type': 'application/json' },
  // });
  // expect(getRes3.status(), { message: `Expected status not 200 for tenant get after delete, but got ${getRes3.status()}` }).not.toBe(200);
  // const getBody3 = await getRes3.json();
  // expect(getBody3.result, { message: `Expected result to be error after delete, but got ${getBody3.result}` }).toBe('error');
  // expect(getBody3.message, { message: `Expected message to be '0 rows found' after delete, but got ${getBody3.message}` }).toBe('0 rows found');
});