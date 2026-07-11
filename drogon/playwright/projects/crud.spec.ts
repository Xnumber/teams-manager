import { expect, request } from '@playwright/test';
import { test } from "../fixture/fixture";
import { projectCreateData, PROJECT_BASE_URL, PROJECT_SINGLE_URL, PROJECT_NAME_UPDATE, PROJECT_DESCRIPTION_UPDATE, getProjectUpdateData, TENANT_NAME, TENANT_DESC, projectCreateData2 } from './data';
import { baseUrl } from '../const';

test.describe.serial('Project CRUD Tests', async () => {
    let project_id: string;
    let getBody: any;
    // let tenant_id: string;
    const name_update = PROJECT_NAME_UPDATE;
    const description_update = PROJECT_DESCRIPTION_UPDATE;


    // test('Create Tenant', async () => {
    //     const apiContext = await request.newContext();
    //     const createTenantRes = await apiContext.post(`${baseUrl}/tenant`, {
    //         data: {
    //             name: TENANT_NAME,
    //             description: TENANT_DESC,
    //         },
    //         headers: { 'Content-Type': 'application/json' },
    //     });
    //     expect(createTenantRes.status(), { message: `Expected status 200 for tenant creation, but got ${createTenantRes.status()}` }).toBe(200);
    //     const createTenantBody = await createTenantRes.json();
    //     tenant_id = createTenantBody.tenant.id;
    //     expect(createTenantBody.tenant.name, { message: `Expected tenant name to be ${TENANT_NAME}, but got ${createTenantBody.tenant.name}` }).toBe(TENANT_NAME);
    //     expect(createTenantBody.tenant.description, { message: `Expected tenant description to be ${TENANT_DESC}, but got ${createTenantBody.tenant.description}` }).toBe(TENANT_DESC);
    // });

    test('新增一個專案', async ({ apiContext, user_1: { tenant_id } }) => {
        // 動態組合 projectCreateData 並帶入 tenant_id
        const createRes = await apiContext.post(PROJECT_BASE_URL, {
            data: { ...projectCreateData },
        });
        console.log('Create Response:', await createRes.text());
        expect(createRes.status(), { message: `Expected status 200 for project creation, but got ${createRes.status()}` }).toBe(200);

        const createBody = await createRes.json();
        expect(createBody.result, { message: `Expected result to be ok, but got ${createBody.result}` }).toBe('ok');
        project_id = createBody.data.id;
        expect(createBody.data.name, { message: `Expected name to be ${projectCreateData.name}, but got ${createBody.data.name}` }).toBe(projectCreateData.name);
        expect(createBody.data.description, { message: `Expected description to be ${projectCreateData.description}, but got ${createBody.data.description}` }).toBe(projectCreateData.description);
        expect(createBody.data.tenant_id, { message: `Expected tenant_id to be ${tenant_id}, but got ${createBody.data.tenant_id}` }).toBe(tenant_id);
    });

    test('新增另一個專案', async ({ apiContext2, user_2: { tenant_id } }) => {
        // 動態組合 projectCreateData 並帶入 tenant_id
        const createRes = await apiContext2.post(PROJECT_BASE_URL, {
            data: { ...projectCreateData2 },
        });
        console.log('Create Response:', await createRes.text());
        expect(createRes.status(), { message: `Expected status 200 for project creation, but got ${createRes.status()}` }).toBe(200);

        const createBody = await createRes.json();
        expect(createBody.result, { message: `Expected result to be ok, but got ${createBody.result}` }).toBe('ok');
        // project_id = createBody.data.id;
        expect(createBody.data.name, { message: `Expected name to be ${projectCreateData2.name}, but got ${createBody.data.name}` }).toBe(projectCreateData2.name);
        expect(createBody.data.description, { message: `Expected description to be ${projectCreateData2.description}, but got ${createBody.data.description}` }).toBe(projectCreateData2.description);
        expect(createBody.data.tenant_id, { message: `Expected tenant_id to be ${tenant_id}, but got ${createBody.data.tenant_id}` }).toBe(tenant_id);
    });

    test('Get', async ({ apiContext }) => {
        const getRes = await apiContext.get(PROJECT_SINGLE_URL(project_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(getRes.status(), { message: `Expected status 200 for project get, but got ${getRes.status()}` }).toBe(200);
        getBody = await getRes.json();
        expect(getBody.result, { message: `Expected result to be ok, but got ${getBody.result}` }).toBe('ok');
        expect(getBody.data.id, { message: `Expected id to be ${project_id}, but got ${getBody.data.id}` }).toBe(project_id);
        expect(getBody.data.name, { message: `Expected name to be ${projectCreateData.name}, but got ${getBody.data.name}` }).toBe(projectCreateData.name);
        expect(getBody.data.description, { message: `Expected description to be ${projectCreateData.description}, but got ${getBody.data.description}` }).toBe(projectCreateData.description);
    });

    test('List', async ({ apiContext }) => {
        const listRes = await apiContext.get(PROJECT_BASE_URL, {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(listRes.status(), { message: `Expected status 200 for project list, but got ${listRes.status()}` }).toBe(200);
        const listBody = await listRes.json();
        expect(listBody.result, { message: `Expected result to be ok, but got ${listBody.result}` }).toBe('ok');
        expect(Array.isArray(listBody.projects), { message: 'Expected projects to be an array' }).toBe(true);
        expect(typeof listBody.count, { message: 'Expected count to be a number' }).toBe('number');
        const filtered = listBody.projects.filter((p: any) =>
            p.id === project_id && p.name === projectCreateData.name && p.description === projectCreateData.description
        );
        expect(filtered.length, { message: `Expected filtered projects to include created project, but got ${filtered.length}` }).toBeGreaterThan(0);
    });

    test('Update', async ({ apiContext }) => {
        const concurrency_stamp = getBody.data.concurrency_stamp;
        const updateRes = await apiContext.put(PROJECT_SINGLE_URL(project_id), {
            data: getProjectUpdateData(concurrency_stamp),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(updateRes.status(), { message: `Expected status 200 for project update, but got ${updateRes.status()}` }).toBe(200);
        const updateBody = await updateRes.json();
        expect(updateBody.result, { message: `Expected result to be ok, but got ${updateBody.result}` }).toBe('ok');
        expect(updateBody.data.id, { message: `Expected id to be ${project_id}, but got ${updateBody.data.id}` }).toBe(project_id);
        expect(updateBody.data.name, { message: `Expected name to be ${name_update}, but got ${updateBody.data.name}` }).toBe(name_update);
        expect(updateBody.data.description, { message: `Expected description to be ${description_update}, but got ${updateBody.data.description}` }).toBe(description_update);
        expect(updateBody.data.concurrency_stamp, { message: `Expected concurrency_stamp to change after update, but got ${updateBody.data.concurrency_stamp}` }).not.toBe(concurrency_stamp);
    });

    test('Get After Update', async ({ apiContext }) => {
        const getRes2 = await apiContext.get(PROJECT_SINGLE_URL(project_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        const getBody2 = await getRes2.json();
        expect(getRes2.status(), { message: `Expected status 200 for project get after update, but got ${getRes2.status()}` }).toBe(200);
        expect(getBody2.result, { message: `Expected result to be ok, but got ${getBody2.result}` }).toBe('ok');
        expect(getBody2.data.id, { message: `Expected id to be ${project_id}, but got ${getBody2.data.id}` }).toBe(project_id);
        expect(getBody2.data.name, { message: `Expected name to be ${name_update}, but got ${getBody2.data.name}` }).toBe(name_update);
        expect(getBody2.data.description, { message: `Expected description to be ${description_update}, but got ${getBody2.data.description}` }).toBe(description_update);
        expect(getBody2.data.concurrency_stamp, { message: `Expected concurrency_stamp to change after update, but got ${getBody2.data.concurrency_stamp}` }).not.toBe(getBody.data.concurrency_stamp);
    });

    test('Delete', async ({ apiContext }) => {
        const deleteRes = await apiContext.delete(PROJECT_SINGLE_URL(project_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(deleteRes.status(), { message: `Expected status 200 for project delete, but got ${deleteRes.status()}` }).toBe(200);
        const deleteBody = await deleteRes.json();
        expect(deleteBody.result, { message: `Expected result to be ok, but got ${deleteBody.result}` }).toBe('ok');
        expect(deleteBody.project.id, { message: `Expected id to be ${project_id}, but got ${deleteBody.project.id}` }).toBe(project_id);
    });

    test('Get After Delete', async ({ apiContext }) => {
        const getRes3 = await apiContext.get(PROJECT_SINGLE_URL(project_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(getRes3.status(), { message: `Expected status not 200 for project get after delete, but got ${getRes3.status()}` }).not.toBe(200);
        const getBody3 = await getRes3.json();
        expect(getBody3.result, { message: `Expected result to be error after delete, but got ${getBody3.result}` }).toBe('error');
        expect(getBody3.message, { message: `Expected message to be '0 rows found' after delete, but got ${getBody3.message}` }).toBe('0 rows found');
    });
});
