import { expect, request } from '@playwright/test';
import { test } from "../fixture/fixture";
import { Role } from './role';

import { ROLE_BASE_URL, ROLE_DESCRIPTION_UPDATE, ROLE_NAME_UPDATE, ROLE_SINGLE_URL, roleCreateData } from "./data"




function getRoleUpdateData() {
  return {
    name: ROLE_NAME_UPDATE,
    description: ROLE_DESCRIPTION_UPDATE,
  };
}

test.describe.serial('Role CRUD Tests', async () => {
  let role_id: string;
  let getBody: any;

  test('Create', async ({ apiContext }) => {
    const createRes = await apiContext.post(ROLE_BASE_URL, {
      data: { ...roleCreateData },
    });
    expect(createRes.status(), { message: `Expected status 200 for role creation, but got ${createRes.status()}` }).toBe(200);
    const createBody = await createRes.json();
    expect(createBody.result, { message: `Expected result to be ok, but got ${createBody.result}` }).toBe('ok');
    role_id = createBody.data.id;
    expect(createBody.data.name, { message: `Expected name to be ${roleCreateData.name}, but got ${createBody.data.name}` }).toBe(roleCreateData.name);
    expect(createBody.data.description, { message: `Expected description to be ${roleCreateData.description}, but got ${createBody.data.description}` }).toBe(roleCreateData.description);
  });

  test('Get', async ({ apiContext }) => {
    const getRes = await apiContext.get(ROLE_SINGLE_URL(role_id), {
      headers: { 'Content-Type': 'application/json' },
    });
    expect(getRes.status(), { message: `Expected status 200 for role get, but got ${getRes.status()}` }).toBe(200);
    getBody = await getRes.json();
    expect(getBody.result, { message: `Expected result to be ok, but got ${getBody.result}` }).toBe('ok');
    expect(getBody.data.id, { message: `Expected id to be ${role_id}, but got ${getBody.data.id}` }).toBe(role_id);
    expect(getBody.data.name, { message: `Expected name to be ${roleCreateData.name}, but got ${getBody.data.name}` }).toBe(roleCreateData.name);
    expect(getBody.data.description, { message: `Expected description to be ${roleCreateData.description}, but got ${getBody.data.description}` }).toBe(roleCreateData.description);
  });

  test('List', async ({ apiContext }) => {
    const listRes = await apiContext.get(ROLE_BASE_URL, {
      headers: { 'Content-Type': 'application/json' },
    });
    expect(listRes.status(), { message: `Expected status 200 for role list, but got ${listRes.status()}` }).toBe(200);
    const listBody = await listRes.json();
    expect(listBody.result, { message: `Expected result to be ok, but got ${listBody.result}` }).toBe('ok');
    expect(Array.isArray(listBody.roles), { message: 'Expected roles to be an array' }).toBe(true);
    expect(typeof listBody.count, { message: 'Expected count to be a number' }).toBe('number');
    const filtered = listBody.roles.filter((r: any) =>
      r.id === role_id && r.name === roleCreateData.name && r.description === roleCreateData.description
    );
    expect(filtered.length, { message: `Expected filtered roles to include created role, but got ${filtered.length}` }).toBeGreaterThan(0);
  });

  test('Update', async ({ apiContext }) => {
    const updateRes = await apiContext.put(ROLE_SINGLE_URL(role_id), {
      data: getRoleUpdateData(),
      headers: { 'Content-Type': 'application/json' },
    });
    expect(updateRes.status(), { message: `Expected status 200 for role update, but got ${updateRes.status()}` }).toBe(200);
    const updateBody = await updateRes.json();
    expect(updateBody.result, { message: `Expected result to be ok, but got ${updateBody.result}` }).toBe('ok');
    expect(updateBody.data.id, { message: `Expected id to be ${role_id}, but got ${updateBody.data.id}` }).toBe(role_id);
    expect(updateBody.data.name, { message: `Expected name to be ${ROLE_NAME_UPDATE}, but got ${updateBody.data.name}` }).toBe(ROLE_NAME_UPDATE);
    expect(updateBody.data.description, { message: `Expected description to be ${ROLE_DESCRIPTION_UPDATE}, but got ${updateBody.data.description}` }).toBe(ROLE_DESCRIPTION_UPDATE);
  });

  test('Get After Update', async ({ apiContext }) => {
    const getRes2 = await apiContext.get(ROLE_SINGLE_URL(role_id), {
      headers: { 'Content-Type': 'application/json' },
    });
    const getBody2 = await getRes2.json();
    expect(getRes2.status(), { message: `Expected status 200 for role get after update, but got ${getRes2.status()}` }).toBe(200);
    expect(getBody2.result, { message: `Expected result to be ok, but got ${getBody2.result}` }).toBe('ok');
    expect(getBody2.data.id, { message: `Expected id to be ${role_id}, but got ${getBody2.data.id}` }).toBe(role_id);
    expect(getBody2.data.name, { message: `Expected name to be ${ROLE_NAME_UPDATE}, but got ${getBody2.data.name}` }).toBe(ROLE_NAME_UPDATE);
    expect(getBody2.data.description, { message: `Expected description to be ${ROLE_DESCRIPTION_UPDATE}, but got ${getBody2.data.description}` }).toBe(ROLE_DESCRIPTION_UPDATE);
  });

  test('Delete', async ({ apiContext }) => {
    const deleteRes = await apiContext.delete(ROLE_SINGLE_URL(role_id), {
      headers: { 'Content-Type': 'application/json' },
    });
    expect(deleteRes.status(), { message: `Expected status 200 for role delete, but got ${deleteRes.status()}` }).toBe(200);
    const deleteBody = await deleteRes.json();
    expect(deleteBody.result, { message: `Expected result to be ok, but got ${deleteBody.result}` }).toBe('ok');
    expect(deleteBody.role.id, { message: `Expected id to be ${role_id}, but got ${deleteBody.role.id}` }).toBe(role_id);
  });

  test('Get After Delete', async ({ apiContext }) => {
    const getRes3 = await apiContext.get(ROLE_SINGLE_URL(role_id), {
      headers: { 'Content-Type': 'application/json' },
    });
    expect(getRes3.status(), { message: `Expected status not 200 for role get after delete, but got ${getRes3.status()}` }).not.toBe(200);
    const getBody3 = await getRes3.json();
    expect(getBody3.result, { message: `Expected result to be error after delete, but got ${getBody3.result}` }).toBe('error');
    expect(getBody3.message, { message: `Expected message to be '0 rows found' after delete, but got ${getBody3.message}` }).toBe('0 rows found');
  });
});
