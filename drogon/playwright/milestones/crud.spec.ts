import { expect } from '@playwright/test';
import { test } from '../fixture/fixture';
import { projectCreateData, PROJECT_BASE_URL, PROJECT_SINGLE_URL, PROJECT_NAME_UPDATE, PROJECT_DESCRIPTION_UPDATE, getProjectUpdateData, TENANT_NAME, TENANT_DESC, projectCreateData2 } from './data';
import { baseUrl } from '../const';

test.describe.serial('Milestone CRUD Tests', async () => {
    let milestoneId: string;
    let createdMilestone: any;

    let concurrency_stamp: string;
    test('新增工作大項', async ({ apiContext2, preparedData }) => {
        const milestoneData = {
            name: 'Playwright 測試工作大項',
            description: '自動化測試建立',
            project_id: preparedData.project.id,
            team_id: preparedData.team.id,
            tenant_id: preparedData.tenant.id,
            start_date: '2026-04-30',
            end_date: '2026-05-28'
        };
        const resp = await apiContext2.post(`${baseUrl}/milestones`, {
            data: milestoneData
        });
        const body = await resp.json();
        expect(resp.status()).toBe(200);
        expect(body.data).toBeDefined();
        expect(body.data.name).toBe(milestoneData.name);
        milestoneId = body.data.id;
        createdMilestone = body.data;
        concurrency_stamp = body.data.concurrency_stamp;
    });

    test('查詢工作大項', async ({ apiContext2 }) => {
        const resp = await apiContext2.get(`${baseUrl}/milestones/${milestoneId}`);
        const body = await resp.json();
        expect(resp.status()).toBe(200);
        expect(body.data).toBeDefined();
        expect(body.data.id).toBe(milestoneId);
        expect(body.data.name).toBe(createdMilestone.name);
    });

    test('更新工作大項', async ({ apiContext2 }) => {
        const updateData = {
            name: 'Playwright 測試工作大項-更新',
            description: '自動化測試更新',
            project_id: createdMilestone.project_id,
            team_id: createdMilestone.team_id,
            tenant_id: createdMilestone.tenant_id,
            start_date: '2026-05-01',
            end_date: '2026-05-30',
            concurrency_stamp: concurrency_stamp
        };
        const resp = await apiContext2.put(`${baseUrl}/milestones/${milestoneId}`, {
            data: updateData
        });
        const body = await resp.json();
        expect(resp.status()).toBe(200);
        expect(body.data).toBeDefined();
        expect(body.data.name).toBe(updateData.name);
        expect(body.data.description).toBe(updateData.description);
        expect(body.data.start_date).toBe(updateData.start_date);
        expect(body.data.end_date).toBe(updateData.end_date);
    });

    test('删除工作大項', async ({ apiContext2 }) => {
        const resp = await apiContext2.delete(`${baseUrl}/milestones/${milestoneId}`);
        expect(resp.status()).toBe(200);
        // 再查詢一次確認已刪除
        const checkResp = await apiContext2.get(`${baseUrl}/milestones/${milestoneId}`);
        expect(checkResp.status()).toBe(500);
    });
});