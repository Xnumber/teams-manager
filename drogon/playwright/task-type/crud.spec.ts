import { test, expect, request } from '@playwright/test';
import { taskTypeCreateData, TASK_TYPE_BASE_URL, TASK_TYPE_SINGLE_URL, TASK_TYPE_NAME_UPDATE, TASK_TYPE_DESCRIPTION_UPDATE, getTaskTypeUpdateData } from './data';

test.describe.serial('TaskType - 工作類別 CRUD 全流程', async () => {
    let task_type_id: string;
    let getBody: any; // 用於在多個測試中共享 getBody 以獲取 concurrency_stamp

    const name_update = TASK_TYPE_NAME_UPDATE;
    const description_update = TASK_TYPE_DESCRIPTION_UPDATE;

    test('Create', async () => {
        const apiContext = await request.newContext();
        const createRes = await apiContext.post(TASK_TYPE_BASE_URL, {
            data: taskTypeCreateData,
            headers: { 'Content-Type': 'application/json' },
        });
        // console.log('Create Response:', await createRes.text());
        expect(createRes.status(), { message: `Expected status 200 for task type creation, but got ${createRes.status()}` }).toBe(200);

        const createBody = await createRes.json();
        expect(createBody.result, { message: `Expected result to be ok, but got ${createBody.result}` }).toBe('ok');
        expect(createBody.data, { message: 'Expected data to be truthy' }).toBeTruthy();
        const created = createBody.data;
        task_type_id = created.id;
        expect(created.name, { message: `Expected name to be ${taskTypeCreateData.name}, but got ${created.name}` }).toBe(taskTypeCreateData.name);
        expect(created.description, { message: `Expected description to be ${taskTypeCreateData.description}, but got ${created.description}` }).toBe(taskTypeCreateData.description);
    });

    test('Get', async () => {
        const apiContext = await request.newContext();
        // 查詢單一 task type
        const getRes = await apiContext.get(TASK_TYPE_SINGLE_URL(task_type_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        getBody = await getRes.json();
        expect(getRes.status(), { message: `Expected status 200 for task type get, but got ${getRes.status()}` }).toBe(200);
        expect(getBody.result, { message: `Expected result to be ok, but got ${getBody.result}` }).toBe('ok');
        expect(getBody.data, { message: 'Expected data to be truthy' }).toBeTruthy();
        const got = getBody.data;
        expect(got.id, { message: `Expected id to be ${task_type_id}, but got ${got.id}` }).toBe(task_type_id);
        expect(got.name, { message: `Expected name to be ${taskTypeCreateData.name}, but got ${got.name}` }).toBe(taskTypeCreateData.name);
        expect(got.description, { message: `Expected description to be ${taskTypeCreateData.description}, but got ${got.description}` }).toBe(taskTypeCreateData.description);
    });

    test('List', async () => {
        const apiContext = await request.newContext();
        const listRes = await apiContext.get(TASK_TYPE_BASE_URL, {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(listRes.status(), { message: `Expected status 200 for task type list, but got ${listRes.status()}` }).toBe(200);
        const listBody = await listRes.json();
        expect(listBody.result, { message: `Expected result to be ok, but got ${listBody.result}` }).toBe('ok');
        expect(Array.isArray(listBody.task_types), { message: 'Expected task_types to be an array' }).toBe(true);
        expect(typeof listBody.count, { message: 'Expected count to be a number' }).toBe('number');
        const filtered = listBody.task_types.filter((t: any) =>
            t.id === task_type_id && t.name === taskTypeCreateData.name && t.description === taskTypeCreateData.description
        );
        expect(filtered.length, { message: `Expected filtered task_types to include created task type, but got ${filtered.length}` }).toBeGreaterThan(0);
    })

    test('Update', async () => {
        const apiContext = await request.newContext();
        // 更新 task type
        const concurrency_stamp = getBody.data.concurrency_stamp;

        const updateRes = await apiContext.put(TASK_TYPE_SINGLE_URL(task_type_id), {
            data: getTaskTypeUpdateData(concurrency_stamp),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(updateRes.status(), { message: `Expected status 200 for task type update, but got ${updateRes.status()}` }).toBe(200);
        const updateBody = await updateRes.json();
        expect(updateBody.result, { message: `Expected result to be ok, but got ${updateBody.result}` }).toBe('ok');
        expect(Array.isArray(updateBody.data), { message: 'Expected data to be an array' }).toBe(true);
        expect(updateBody.data.length, { message: 'Expected data array to have 1 element' }).toBe(1);
        const updated = updateBody.data[0];
        expect(updated.id, { message: `Expected id to be ${task_type_id}, but got ${updated.id}` }).toBe(task_type_id);
        expect(updated.name, { message: `Expected name to be ${name_update}, but got ${updated.name}` }).toBe(name_update);
        expect(updated.description, { message: `Expected description to be ${description_update}, but got ${updated.description}` }).toBe(description_update);
        expect(updated.concurrency_stamp, { message: `Expected concurrency_stamp to change after update, but got ${updated.concurrency_stamp}` }).not.toBe(concurrency_stamp);
    })

    test('Get After Update', async () => {
        const apiContext = await request.newContext();
        // 更新後再次查詢
        const getRes2 = await apiContext.get(TASK_TYPE_SINGLE_URL(task_type_id), {
            headers: { 'Content-Type': 'application/json' },
        });

        expect(getRes2.status(), { message: `Expected status 200 for task type get after update, but got ${getRes2.status()}` }).toBe(200);
        const getBody2 = await getRes2.json();
        expect(getBody2.result, { message: `Expected result to be ok, but got ${getBody2.result}` }).toBe('ok');
        expect(getBody2.data, { message: 'Expected data to be truthy' }).toBeTruthy();
        const got2 = getBody2.data;
        expect(got2.id, { message: `Expected id to be ${task_type_id}, but got ${got2.id}` }).toBe(task_type_id);
        expect(got2.name, { message: `Expected name to be ${name_update}, but got ${got2.name}` }).toBe(name_update);
        expect(got2.description, { message: `Expected description to be ${description_update}, but got ${got2.description}` }).toBe(description_update);
        expect(got2.concurrency_stamp, { message: `Expected concurrency_stamp to change after update, but got ${got2.concurrency_stamp}` }).not.toBe(getBody.concurrency_stamp);
    })

    test('Delete', async () => {
        const apiContext = await request.newContext();
        
        // 刪除 task type
        const deleteRes = await apiContext.delete(TASK_TYPE_SINGLE_URL(task_type_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        const deleteBody = await deleteRes.json();
        expect(deleteRes.status(), { message: `Expected status 200 for task type delete, but got ${deleteRes.status()}` }).toBe(200);
    })
    
    test('Get After Delete', async () => {
        const apiContext = await request.newContext();
        // 刪除後再次查詢
        const getRes3 = await apiContext.get(TASK_TYPE_SINGLE_URL(task_type_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(getRes3.status(), { message: `Expected status not 200 for task type get after delete, but got ${getRes3.status()}` }).not.toBe(200);
        const getBody3 = await getRes3.json();
        expect(getBody3.result, { message: `Expected result to be error after delete, but got ${getBody3.result}` }).toBe('error');
        expect(getBody3.message, { message: `Expected message to be '0 rows found' after delete, but got ${getBody3.message}` }).toBe('0 rows found');
    });
});