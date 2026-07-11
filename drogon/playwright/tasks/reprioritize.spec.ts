import { expect } from '@playwright/test';
import { test } from '../fixture/fixture';
import { getProjectCreateData, getTaskTypeCreateData, getThisWeekTaskCreateData, TASK_BASE_URL } from './data';

const TASK_REPRIORITIZE_URL = 'http://localhost:5001/task/reprioritize';

test.describe.serial('Task Reprioritize API Tests', () => {
    let projectId: string;
    let taskTypeId: string;
    const createdTaskIds: string[] = [];

    test('Prepare project and task type', async ({ apiContext }) => {
        const taskTypeRes = await apiContext.post('http://localhost:5001/task-types', {
            data: getTaskTypeCreateData(),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(taskTypeRes.status()).toBe(200);
        const taskTypeBody = await taskTypeRes.json();
        expect(taskTypeBody.result).toBe('ok');
        taskTypeId = taskTypeBody.data.id;

        const projectRes = await apiContext.post('http://localhost:5001/projects', {
            data: getProjectCreateData(),
            headers: { 'Content-Type': 'application/json' },
        });
        expect(projectRes.status()).toBe(200);
        const projectBody = await projectRes.json();
        expect(projectBody.result).toBe('ok');
        projectId = projectBody.data.id;
    });

    test('Create 3 tasks with explicit priorities', async ({ apiContext, user_1, taskStatusList, teams }) => {
        const baseData = getThisWeekTaskCreateData(
            projectId,
            taskTypeId,
            [user_1.id],
            [user_1.id],
            taskStatusList,
            teams[0].id
        );

        for (let i = 1; i <= 3; i++)
        {
            const createRes = await apiContext.post(TASK_BASE_URL, {
                data: {
                    ...baseData,
                    name: `reprioritize-task-${i}-${Date.now()}`,
                    priority: i,
                },
                headers: { 'Content-Type': 'application/json' },
            });
            expect(createRes.status()).toBe(200);
            const createBody = await createRes.json();
            expect(createBody.result).toBe('ok');
            createdTaskIds.push(createBody.data.id);
        }

        expect(createdTaskIds.length).toBe(3);
    });

    test('PATCH reprioritize should move task to top', async ({ apiContext }) => {
        const targetTaskId = createdTaskIds[2];

        const reprioritizeRes = await apiContext.patch(TASK_REPRIORITIZE_URL, {
            data: {
                task_id: targetTaskId,
                priority: 1,
            },
            headers: { 'Content-Type': 'application/json' },
        });

        expect(reprioritizeRes.status()).toBe(200);
        const reprioritizeBody = await reprioritizeRes.json();
        expect(reprioritizeBody.result).toBe('ok');

        const listRes = await apiContext.get(TASK_BASE_URL);
        expect(listRes.status()).toBe(200);
        const listBody = await listRes.json();
        expect(listBody.result).toBe('ok');

        const targetTasks = listBody.tasks
            .filter((t: any) => createdTaskIds.includes(t.id))
            .map((t: any) => ({ id: t.id, priority: Number(t.priority) }))
            .sort((a: any, b: any) => a.priority - b.priority);

        expect(targetTasks.length).toBe(3);
        expect(targetTasks[0].id).toBe(createdTaskIds[2]);
        expect(targetTasks[0].priority).toBe(1);
    });

    test('PUT reprioritize should still work for compatibility', async ({ apiContext }) => {
        const targetTaskId = createdTaskIds[0];

        const reprioritizeRes = await apiContext.put(TASK_REPRIORITIZE_URL, {
            data: {
                task_id: targetTaskId,
                priority: 3,
            },
            headers: { 'Content-Type': 'application/json' },
        });

        expect(reprioritizeRes.status()).toBe(200);
        const reprioritizeBody = await reprioritizeRes.json();
        expect(reprioritizeBody.result).toBe('ok');

        const listRes = await apiContext.get(TASK_BASE_URL);
        expect(listRes.status()).toBe(200);
        const listBody = await listRes.json();
        expect(listBody.result).toBe('ok');

        const targetTasks = listBody.tasks
            .filter((t: any) => createdTaskIds.includes(t.id))
            .map((t: any) => ({ id: t.id, priority: Number(t.priority) }))
            .sort((a: any, b: any) => a.priority - b.priority);

        expect(targetTasks.length).toBe(3);
        expect(targetTasks[2].id).toBe(createdTaskIds[0]);
        expect(targetTasks[2].priority).toBe(3);
    });
});
