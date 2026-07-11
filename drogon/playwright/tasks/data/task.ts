// 工作相關測試資料
import { host } from '../../const/enviroment';
import { PLAYWRIGHTTESTUSER } from './user';

export const TASK_BASE_URL = `${host}/tasks`;
export const TASK_COMPLETE_URL = (id: string) => `${host}/set-task-completed/${id}`;
export const TASK_SINGLE_URL = (id: string) => `${TASK_BASE_URL}/${id}`;
export const getThisWeekTaskCreateData = (
    // tenant_id: string | null = null,
    // creator_id: string | null = null,
    project_id: string | null = null,
    task_type_id: string | null = null,
    executor_ids: string[] = [],
    mentor_ids: string[] = [],
    taskStatus: { id: string }[],
    team_id: string | null = null
) => ({
    // tenant_id: tenant_id,
    // creator_id: creator_id,
    // creator_name: PLAYWRIGHTTESTUSER,
    name: 'This Week Playwright 測試工作',
    description: 'Playwright 測試用工作描述',
    // scheduled_start_date: '2026-03-01',
    // scheduled_end_date: '2026-03-10',
    this_week: true,
    project_id: project_id,
    ececutor_time_ratio: 0.5,
    task_type_id: task_type_id,
    executor_ids: executor_ids,
    mentor_ids: mentor_ids,
    status_id: taskStatus[0].id, // 預設使用第一個狀態
    team_id: team_id,
    estimated_workdays: 3
});
export const getNextWeekTaskCreateData = (
    // tenant_id: string | null = null,
    // creator_id: string | null = null,
    project_id: string | null = null,
    task_type_id: string | null = null,
    executor_ids: string[] = [],
    mentor_ids: string[] = [],
    taskStatus: { id: string }[],
    team_id: string | null = null
) => ({
    // tenant_id: tenant_id,
    // creator_id: creator_id,
    // creator_name: PLAYWRIGHTTESTUSER,
    name: 'Next Week Playwright 測試工作',
    description: 'Playwright 測試用工作描述',
    // scheduled_start_date: '2026-03-01',
    // scheduled_end_date: '2026-03-10',
    this_week: false,
    project_id: project_id,
    task_type_id: task_type_id,
    executor_ids: executor_ids,
    mentor_ids: mentor_ids,
    ececutor_time_ratio: 0.5,
    status_id: taskStatus[1].id, // 預設使用第二個狀態
    team_id: team_id,
    estimated_workdays: 3
});
