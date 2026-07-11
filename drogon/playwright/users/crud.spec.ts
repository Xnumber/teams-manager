import { expect, request } from '@playwright/test';
import { test } from "../fixture/fixture"
test('User CRUD 全流程', async ({ apiContext, teams }) => {
	// const apiContext = await request.newContext();

	// 先建立一個租戶，取得 tenant_id
	const tenantRes = await apiContext.post('http://localhost:5001/tenant', {
		data: {
			name: 'UserTestTenant',
			description: 'UserTestTenantDesc',
		},
		headers: { 'Content-Type': 'application/json' },
	});
	expect(tenantRes.status(), { message: `Expected status 200 for tenant creation, but got ${tenantRes.status()}` }).toBe(200);
	const tenantBody = await tenantRes.json();
	const tenant_id = tenantBody.tenant.id;

	// 建立 user
	const username_send = 'testuser';
	const email_send = 'testuser@example.com';
	const password_send = 'testpassword';
	const createRes = await apiContext.post('http://localhost:5001/user', {
		data: {
			tenant_id: tenant_id,
			team_id: teams[0].id,
			team_name: teams[0].name,
			username: username_send,
			email: email_send,
			password: password_send,
		},
		headers: { 'Content-Type': 'application/json' },
	});
	expect(createRes.status(), { message: `Expected status 200 for user creation, but got ${createRes.status()}` }).toBe(200);
	const createBody = await createRes.json();
	// console.log('Create User Response:', createBody);
	// console.log('Create User Response:', createBody);
	// console.log('Create User Response:', createBody);
	// console.log('Create User Response:', createBody);
	// console.log('Create User Response:', createBody);
	// console.log('Create User Response:', createBody);
	// console.log('Create User Response:', createBody);
	const user_id = createBody.data.id;
	expect(createBody.data.tenant_id, { message: `Expected user tenant_id to be ${tenant_id}, but got ${createBody.data.tenant_id}` }).toBe(tenant_id);
	expect(createBody.data.username, { message: `Expected user username to be ${username_send}, but got ${createBody.data.username}` }).toBe(username_send);
	expect(createBody.data.email, { message: `Expected user email to be ${email_send}, but got ${createBody.data.email}` }).toBe(email_send);

	// 查詢單一 user
	const getRes = await apiContext.get(`http://localhost:5001/user/${user_id}`, {
		headers: { 'Content-Type': 'application/json' },
	});
	expect(getRes.status(), { message: `Expected status 200 for user get, but got ${getRes.status()}` }).toBe(200);
	const getBody = await getRes.json();
	expect(getBody.result, { message: `Expected result to be ok, but got ${getBody.result}` }).toBe('ok');
	expect(getBody.data.id, { message: `Expected user id to be ${user_id}, but got ${getBody.data.id}` }).toBe(user_id);
	expect(getBody.data.tenant_id, { message: `Expected user tenant_id to be ${tenant_id}, but got ${getBody.data.tenant_id}` }).toBe(tenant_id);
	expect(getBody.data.username, { message: `Expected user username to be ${username_send}, but got ${getBody.data.username}` }).toBe(username_send);
	expect(getBody.data.email, { message: `Expected user email to be ${email_send}, but got ${getBody.data.email}` }).toBe(email_send);
	expect(getBody.data.password, { message: `Expected user password to be ${password_send}, but got ${getBody.data.password}` }).toBe(password_send);

	// 查詢 user 列表並過濾
	const listRes = await apiContext.get('http://localhost:5001/user', {
		headers: { 'Content-Type': 'application/json' },
	});
	expect(listRes.status(), { message: `Expected status 200 for user list, but got ${listRes.status()}` }).toBe(200);
	const listBody = await listRes.json();
	expect(listBody.result, { message: `Expected result to be ok, but got ${listBody.result}` }).toBe('ok');
	expect(Array.isArray(listBody.data), { message: 'Expected users to be an array' }).toBe(true);
	expect(typeof listBody.count, { message: 'Expected count to be a number' }).toBe('number');
	// console.log('User List Response - Total Users:', listBody);
	// console.log(
	// 	user_id,
	// 	tenant_id,
	// 	username_send,
	// 	email_send
	// )
	const filtered = listBody.data.filter(
		u => u.id === user_id && 
		u.tenant_id === tenant_id && 
		u.name === username_send && 
		u.email === email_send
	);
	
	expect(filtered.length, { message: `Expected filtered users to include created user, but got ${filtered.length}` }).toBe(1);

	// 更新 user
	const concurrency_stamp = getBody.data.concurrency_stamp;
	const username_update = 'updateduser';
	const email_update = 'updateduser@example.com';
	const password_update = 'updatedpassword';
	const updateRes = await apiContext.put(`http://localhost:5001/user/${user_id}`, {
		data: {
			tenant_id: tenant_id,
			username: username_update,
			email: email_update,
			password: password_update,
			concurrency_stamp: concurrency_stamp,
		},
		headers: { 'Content-Type': 'application/json' },
	});
	expect(updateRes.status(), { message: `Expected status 200 for user update, but got ${updateRes.status()}` }).toBe(200);
	const updateBody = await updateRes.json();
	expect(updateBody.data.id, { message: `Expected user id to be ${user_id}, but got ${updateBody.data.id}` }).toBe(user_id);
	expect(updateBody.data.tenant_id, { message: `Expected user tenant_id to be ${tenant_id}, but got ${updateBody.data.tenant_id}` }).toBe(tenant_id);
	expect(updateBody.data.username, { message: `Expected user username to be ${username_update}, but got ${updateBody.data.username}` }).toBe(username_update);
	expect(updateBody.data.email, { message: `Expected user email to be ${email_update}, but got ${updateBody.data.email}` }).toBe(email_update);
	expect(updateBody.data.password, { message: `Expected user password to be ${password_update}, but got ${updateBody.data.password}` }).toBe(password_update);
	expect(updateBody.data.concurrency_stamp, { message: `Expected concurrency_stamp to change after update, but got ${updateBody.data.concurrency_stamp}` }).not.toBe(concurrency_stamp);

	// console.log('Update User Response - user id:', user_id);
	// 更新後再次查詢
	const getRes2 = await apiContext.get(`http://localhost:5001/user/${user_id}`, {
		headers: { 'Content-Type': 'application/json' },
	});
	expect(getRes2.status(), { message: `Expected status 200 for user get after update, but got ${getRes2.status()}` }).toBe(200);
	const getBody2 = await getRes2.json();
	expect(getBody2.result, { message: `Expected result to be ok, but got ${getBody2.result}` }).toBe('ok');
	expect(getBody2.data.id, { message: `Expected user id to be ${user_id}, but got ${getBody2.data.id}` }).toBe(user_id);
	expect(getBody2.data.tenant_id, { message: `Expected user tenant_id to be ${tenant_id}, but got ${getBody2.data.tenant_id}` }).toBe(tenant_id);
	expect(getBody2.data.username, { message: `Expected user username to be ${username_update}, but got ${getBody2.data.username}` }).toBe(username_update);
	expect(getBody2.data.email, { message: `Expected user email to be ${email_update}, but got ${getBody2.data.email}` }).toBe(email_update);
	expect(getBody2.data.password, { message: `Expected user password to be ${password_update}, but got ${getBody2.data.password}` }).toBe(password_update);
	expect(getBody2.data.concurrency_stamp, { message: `Expected concurrency_stamp to change after update, but got ${getBody2.data.concurrency_stamp}` }).not.toBe(concurrency_stamp);

	// 刪除 user
	// const deleteRes = await apiContext.delete(`http://localhost:5001/user/${user_id}`, {
	// 	headers: { 'Content-Type': 'application/json' },
	// });
	// expect(deleteRes.status(), { message: `Expected status 200 for user delete, but got ${deleteRes.status()}` }).toBe(200);
	// const deleteBody = await deleteRes.json();
	// expect(deleteBody.data.id, { message: `Expected user id to be ${user_id}, but got ${deleteBody.data.id}` }).toBe(user_id);

	// // 刪除後再次查詢
	// const getRes3 = await apiContext.get(`http://localhost:5001/user/${user_id}`, {
	// 	headers: { 'Content-Type': 'application/json' },
	// });
	// expect(getRes3.status(), { message: `Expected status not 200 for user get after delete, but got ${getRes3.status()}` }).not.toBe(200);
	// const getBody3 = await getRes3.json();
	// expect(getBody3.result, { message: `Expected result to be error after delete, but got ${getBody3.result}` }).toBe('error');
	// expect(getBody3.message, { message: `Expected message to be '0 rows found' after delete, but got ${getBody3.message}` }).toBe('0 rows found');
});
