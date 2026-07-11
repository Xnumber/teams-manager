import { test, expect, request } from '@playwright/test';
import { teamCreateData, TEAM_BASE_URL, TEAM_SINGLE_URL, TEAM_NAME_UPDATE, TEAM_DESCRIPTION_UPDATE, getTeamUpdateData, TENANT_NAME, TENANT_DESC } from './teams';



test.describe.serial('Teams CRUD 全流程', async () => {
    let team_id: string;
    let getBody: any;
    let tenant_id: string;
    const name_update = TEAM_NAME_UPDATE;
    const description_update = TEAM_DESCRIPTION_UPDATE;

    test('Create Tenant', async () => {
        const apiContext = await request.newContext();
        const createTenantRes = await apiContext.post('http://localhost:5001/tenant', {
            data: {
                name: TENANT_NAME,
                description: TENANT_DESC,
            },
            headers: { 'Content-Type': 'application/json' },
        });
        expect(createTenantRes.status(), { message: `Expected status 200 for tenant creation, but got ${createTenantRes.status()}` }).toBe(200);
        const createTenantBody = await createTenantRes.json();
        tenant_id = createTenantBody.tenant.id;
        expect(createTenantBody.tenant.name, { message: `Expected tenant name to be ${TENANT_NAME}, but got ${createTenantBody.tenant.name}` }).toBe(TENANT_NAME);
        expect(createTenantBody.tenant.description, { message: `Expected tenant description to be ${TENANT_DESC}, but got ${createTenantBody.tenant.description}` }).toBe(TENANT_DESC);
    });

    test('Create', async () => {
        const apiContext = await request.newContext();
        const createRes = await apiContext.post(TEAM_BASE_URL, {
            data: { ...teamCreateData, tenant_id },
            headers: { 'Content-Type': 'application/json' },
        });
        expect(createRes.status(), { message: `Expected status 200 for team creation, but got ${createRes.status()}` }).toBe(200);
        const createBody = await createRes.json();
        // console.log('Create Team Response: 123', createBody.data);
        expect(createBody.result, { message: `Expected result to be ok, but got ${createBody.result}` }).toBe('ok');
        expect(createBody.data.tenant_id, { message: `Expected tenant id to be ${tenant_id}, but got ${createBody.data.tenant_id}` }).toBe(tenant_id);
        expect(createBody.data.tenant_name, { message: `Expected tenant name to be ${TENANT_NAME}, but got ${createBody.data.tenant_name}` }).toBe(TENANT_NAME);
        expect(createBody.data.name, { message: `Expected team name to be ${teamCreateData.name}, but got ${createBody.data.name}` }).toBe(teamCreateData.name);
        expect(createBody.data.description, { message: `Expected team description to be ${teamCreateData.description}, but got ${createBody.data.description}` }).toBe(teamCreateData.description);
        team_id = createBody.data.id;
    });

    test('Get', async () => {
        const apiContext = await request.newContext();
        const getRes = await apiContext.get(TEAM_SINGLE_URL(team_id), {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(getRes.status(), { message: `Expected status 200 for get, but got ${getRes.status()}` }).toBe(200);
        getBody = await getRes.json();
        expect(getBody.result, { message: `Expected result to be ok, but got ${getBody.result}` }).toBe('ok');
        expect(getBody.data.id, { message: `Expected team id to be ${team_id}, but got ${getBody.data.id}` }).toBe(team_id);
    });

    test('Update', async () => {
        const apiContext = await request.newContext();
        const updateRes = await apiContext.put(TEAM_SINGLE_URL(team_id), {
            data: getTeamUpdateData(getBody.data.concurrency_stamp),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(updateRes.status(), { message: `Expected status 200 for update, but got ${updateRes.status()}` }).toBe(200);
        const updateBody = await updateRes.json();
        expect(updateBody.result, { message: `Expected result to be ok, but got ${updateBody.result}` }).toBe('ok');
        expect(updateBody.data.name, { message: `Expected team name to be ${name_update}, but got ${updateBody.data.name}` }).toBe(name_update);
        expect(updateBody.data.description, { message: `Expected team description to be ${description_update}, but got ${updateBody.data.description}` }).toBe(description_update);
    });

    // test('Delete', async () => {
    //     const apiContext = await request.newContext();
    //     const deleteRes = await apiContext.delete(TEAM_SINGLE_URL(team_id), {
    //         headers: { 'Content-Type': 'application/json' },
    //     });
    //     expect(deleteRes.status(), { message: `Expected status 200 for delete, but got ${deleteRes.status()}` }).toBe(200);
    //     const deleteBody = await deleteRes.json();
    //     expect(deleteBody.result, { message: `Expected result to be ok, but got ${deleteBody.result}` }).toBe('ok');
    // });
});
