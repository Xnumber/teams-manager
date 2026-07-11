import { expect, request } from '@playwright/test';
import { TASK_BASE_URL, TASK_SINGLE_URL, getThisWeekTaskCreateData, TENANT_NAME, TENANT_DESC, TENANT_NAME_2, TENANT_DESC_2, TASK_TYPE_NAME, TASK_TYPE_DESC, PROJECT_NAME, PROJECT_DESC, getUserCreateData, getAnotherProjectCreateData, ANOTHER_PROJECT_NAME, TASK_TYPE_NAME_2, getUserCreateData2, PLAYWRIGHTTESTUSER2, getNextWeekTaskCreateData, TASK_COMPLETE_URL } from './data';
import { getTaskTypeCreateData, getTaskTypeCreateData2, getProjectCreateData } from './data';
import { host } from '../const/enviroment';
import { test } from "../fixture/fixture";

test.describe.serial('Task CRUD Tests', () => {
    let task_id: string;
    let concurrency_stamp: string;
    let task_type_id: string;
    let task_type_name: string;
    let task_type_id_2: string;
    let project_id: string;
    let project_name: string;
    let project_id_2: string;
    let tenant_id: string;
    let tenant_id_2: string;
    let tenant_name: string;
    // let tenant_name_2: string;
    let creator_id: string;
    let creator_name: string;
    let user_id_2: string;
    let user_id_3: string;


    

    test('Create TaskType & Project & Tenant & Creator', async ({ apiContext }) => {
        // const apiContext = await request.newContext();
        // // 建立 Tenant
        // const tenantRes = await apiContext.post(`${host}/tenant`, {
        //     data: {
        //         name: TENANT_NAME,
        //         description: TENANT_DESC,
        //     },
        //     headers: { 'Content-Type': 'application/json' },
        // });
        // expect(tenantRes.status()).toBe(200);
        // const tenantBody = await tenantRes.json();
        // expect(tenantBody.result).toBe('ok');
        // tenant_id = tenantBody.tenant.id;
        // tenant_name = tenantBody.tenant.name;

        // // 建立 Creator (User)
        // const userData = getUserCreateData(tenant_id);
        // const creatorRes = await apiContext.post('http://localhost:5001/register', {
        //     data: userData,
        //     headers: { 'Content-Type': 'application/json' },
        // });
        // expect(creatorRes.status()).toBe(200);
        // const creatorBody = await creatorRes.json();
        // expect(creatorBody.data.tenant_id).toBe(tenant_id);
        // expect(creatorBody.data.username).toBe(userData.username);
        // expect(creatorBody.data.email).toBe(userData.email);
        // creator_id = creatorBody.data.id;
        // creator_name = creatorBody.data.username;

        // 建立 TaskType
        const taskTypeRes = await apiContext.post('http://localhost:5001/task-types', {
            data: getTaskTypeCreateData(),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(taskTypeRes.status()).toBe(200);
        const taskTypeBody = await taskTypeRes.json();
        expect(taskTypeBody.result).toBe('ok');
        task_type_id = taskTypeBody.data.id;
        task_type_name = taskTypeBody.data.name;
        // 建立 Project
        const projectRes = await apiContext.post('http://localhost:5001/projects', {
            data: getProjectCreateData(),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(projectRes.status()).toBe(200);
        const projectBody = await projectRes.json();
        expect(projectBody.result).toBe('ok');
        project_id = projectBody.data.id;
        project_name = projectBody.data.name;
    });

    // 額外一組 TaskType & Project & Tenant（不含 Creator）
    test('Create TaskType & Project & Tenant (2nd set, no Creator)', async ({ apiContext, tenant }) => {
        // creator_name = creatorBody.data.username;
        // 建立 TaskType
        const taskTypeRes = await apiContext.post('http://localhost:5001/task-types', {
            data: getTaskTypeCreateData2(),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(taskTypeRes.status()).toBe(200);
        const taskTypeBody = await taskTypeRes.json();
        expect(taskTypeBody.result).toBe('ok');
        task_type_id_2 = taskTypeBody.data.id;

        // 建立 Project
        const projectRes = await apiContext.post('http://localhost:5001/projects', {
            data: getAnotherProjectCreateData(),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(projectRes.status()).toBe(200);
        const projectBody = await projectRes.json();
        expect(projectBody.result).toBe('ok');
        project_id_2 = projectBody.data.id;
    });


    test('Create A This Week Task', async ({ apiContext, user_1, tenant,  taskStatusList, teams }) => {
        // const apiContext = await request.newContext();
        const taskCreateData = getThisWeekTaskCreateData(
            // tenant_id,
            // user_1.id,
            project_id,
            task_type_id,
            [
                user_1.id, // executorIds
            ],
            [
                user_1.id, // mentorIds
            ],
            taskStatusList,
            teams[0].id
        );
        const createRes = await apiContext.post(TASK_BASE_URL, {
            data: taskCreateData,
            headers: { 'Content-Type': 'application/json' },
        });
        expect(createRes.status()).toBe(200);
        const createBody = await createRes.json();
        expect(createBody.result).toBe('ok');
        expect(createBody.data.name).toBe(taskCreateData.name);
        // 新增: 驗證 tenant_name, creator_name, project_name, task_type_name
        // console.log('Create Task Response:', createBody);
        expect(createBody.data.tenant_name).toBe(tenant.name);
        expect(createBody.data.creator_name).toBe(user_1.username);
        expect(createBody.data.project_name).toBe(project_name);
        expect(createBody.data.task_type_name).toBe(task_type_name);
        expect(createBody.data.status_name).toBe(taskStatusList[0].name);
        expect(createBody.data.executors).toEqual([
            expect.objectContaining({
                user_id: user_1.id,
                user_name: user_1.username
            })
        ]);

        expect(createBody.data.mentors).toEqual([
            expect.objectContaining({
                user_id: user_1.id,
                user_name: user_1.username
            })
        ]);
        task_id = createBody.data.id;
        concurrency_stamp = createBody.data.concurrency_stamp;
    });



    test('Create A Next Week Task', async ({ apiContext2, user_2, user_1, tenant, taskStatusList, teams }) => {
        // const apiContext = await request.newContext();
        const taskCreateData = getNextWeekTaskCreateData(
            // tenant_id,
            // user_1.id,
            project_id,
            task_type_id,
            [
                user_1.id, // executorIds
            ],
            [
                user_2.id, // mentorIds
            ],
            taskStatusList,
            teams[0].id
        );
        const createRes = await apiContext2.post(TASK_BASE_URL, {
            data: taskCreateData,
            headers: { 'Content-Type': 'application/json' },
        });
        expect(createRes.status()).toBe(200);
        const createBody = await createRes.json();
        expect(createBody.result).toBe('ok');
        expect(createBody.data.name).toBe(taskCreateData.name);
        // 新增: 驗證 tenant_name, creator_name, project_name, task_type_name
        // console.log('Create A Next Week Task Create Task Response:', createBody);
        expect(createBody.data.tenant_name).toBe(tenant.name);
        expect(createBody.data.creator_name).toBe(user_2.username);
        expect(createBody.data.project_name).toBe(project_name);
        expect(createBody.data.task_type_name).toBe(task_type_name);
        expect(createBody.data.status_name).toBe(taskStatusList[1].name);
        expect(createBody.data.executors).toEqual([
            expect.objectContaining({
                user_id: user_1.id,
                user_name: user_1.username
            })
        ]);

        expect(createBody.data.mentors).toEqual([
            expect.objectContaining({
                user_id: user_2.id,
                user_name: user_2.username
            })
        ]);
        // task_id = createBody.data.id;
        // concurrency_stamp = createBody.data.concurrency_stamp;
    });

    test('Get', async ({ apiContext }) => {
        // const apiContext = await request.newContext();
        const getRes = await apiContext.get(TASK_SINGLE_URL(task_id));
        expect(getRes.status()).toBe(200);
        const getBody = await getRes.json();
        expect(getBody.result).toBe('ok');
        expect(getBody.data.id).toBe(task_id);
    });

    test('Update', async ({ user_1, user_2, tenant, taskStatusList, apiContext }) => {
        const updateData = {
            ...getThisWeekTaskCreateData(
                project_id_2,
                task_type_id_2,
                [user_2.id], // executorIds
                [user_2.id], // mentorIds
                taskStatusList
            ),
            name: 'Playwright 測試工作-更新',
            concurrency_stamp,
        };
        const updateRes = await apiContext.put(TASK_SINGLE_URL(task_id), {
            data: updateData,
            headers: { 'Content-Type': 'application/json' },
        });
        expect(updateRes.status()).toBe(200);
        const updateBody = await updateRes.json();
        expect(updateBody.result).toBe('ok');
        expect(updateBody.data.name).toBe(updateData.name);

        // 驗證更新後的 tenant_name, project_name, task_type_name
        expect(updateBody.data.tenant_name).toBe(tenant.name);
        expect(updateBody.data.project_name).toBe(ANOTHER_PROJECT_NAME);
        expect(updateBody.data.task_type_name).toBe(TASK_TYPE_NAME_2);

        expect(updateBody.data).toBeTruthy();
        expect(updateBody.data.executors).toEqual([
            expect.objectContaining({
                user_id: user_2.id,
                user_name: user_2.username
            })
        ]);
        expect(updateBody.data.mentors).toEqual([
            expect.objectContaining({
                user_id: user_2.id,
                user_name: user_2.username
            })
        ]);
    });

    
    test('設定工作完成', async ({ apiContext }) => {
        const completeRes = await apiContext.put(`${TASK_COMPLETE_URL(task_id)}`, {
            headers: { 'Content-Type': 'application/json' },
        });
        expect(completeRes.status()).toBe(200);
        const completeBody = await completeRes.json();
        const compoletedTaskRes = await apiContext.get(TASK_SINGLE_URL(task_id));
        expect(compoletedTaskRes.status()).toBe(200);
        const completedTaskBody = await compoletedTaskRes.json();
        expect(completedTaskBody.result).toBe('ok');
        expect(completedTaskBody.data.id).toBe(task_id);
        expect(completedTaskBody.data.completion_date).toBeTruthy();
        expect(completedTaskBody.data.completed).toBe(true);
    });

    // test('Delete', async () => {
    //     const apiContext = await request.newContext();
    //     const deleteRes = await apiContext.delete(TASK_SINGLE_URL(task_id));
    //     expect(deleteRes.status()).toBe(200);
    //     const deleteBody = await deleteRes.json();
    //     expect(deleteBody.result).toBe('ok');
    // });

    // test('Get After Delete', async () => {
    //     const apiContext = await request.newContext();
    //     const getRes = await apiContext.get(TASK_SINGLE_URL(task_id));
    //     expect(getRes.status()).not.toBe(200);
    //     const getBody = await getRes.json();
    //     expect(getBody.result).toBe('error');
    //     expect(getBody.message).toBe('0 rows found');
    // });
});