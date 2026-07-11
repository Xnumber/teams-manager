import http from 'k6/http';
import { check } from 'k6';
import { Milestone, Project, UserWithAuthToken } from './type';

type TaskStatus = {
    id: string;
    name: string;
};

type Task = {
    id: string;
    name: string;
    description: string;
    project_id: string;
    milestone_id: string;
    status_id: string;
};

function pickRandom<T>(items: T[]): T | null {
    if (items.length === 0) {
        return null;
    }
    const index = Math.floor(Math.random() * items.length);
    return items[index] ?? null;
}

function getTaskStatuses(): TaskStatus[] {
    const res = http.get('http://localhost:5001/task-status');
    const ok = check(res, {
        'task status list is 200': (r) => r.status === 200,
    });

    if (!ok) {
        throw new Error('Failed to preload task statuses');
    }

    const body = res.json() as { data?: TaskStatus[] };
    return Array.isArray(body?.data) ? body.data : [];
}

/**
 * 為每個使用者在每個專案下創建工作，目前每個使用者在每個專案下創建30個工作
 * @param users 
 * @param projects 
 */
export function prepareTasks(
    users: UserWithAuthToken[], 
    projects: Project[],
    milestones: Milestone[]
): Task[] {
    const tasks: Task[] = [];
    const tasksPerUser = 30;
    const taskStatuses = getTaskStatuses();
    if (taskStatuses.length === 0) {
        throw new Error('No task statuses found, cannot prepare tasks');
    }

    // 取得去重複的project id 陣列，並且可以快速找到對應的專案
    const projectIdsSet = new Set<string>();
    
    // 將專案ID加入Set中，確保唯一性
    projects.forEach(project => {
        projectIdsSet.add(project.id);
    });
    // 建立一個專案與其對應的工作大項的映射，方便後續創建工作時使用
    const projectMilestoneMap: {
        projectId: string;
        milestones: Milestone[];
    }[] = [];
    // 為每個專案找到對應的工作大項，並建立映射
    projectIdsSet.forEach(projectId => {
        const projectMilestones = milestones.filter(milestone => milestone.project_id === projectId);
        if (projectMilestones.length > 0) {
            projectMilestoneMap.push({
                projectId,
                milestones: projectMilestones,
            });
        }
    });

    for (const user of users) {
        const userProjectIds = new Set(
            projects
                .filter((project) => project.creator_name === user.username)
                .map((project) => project.id)
        );
        const userProjectMilestoneMap = projectMilestoneMap.filter((entry) => userProjectIds.has(entry.projectId));

        for (let i = 0; i < tasksPerUser; i++) {
            const pickedProjectEntry = pickRandom(
                userProjectMilestoneMap.length > 0 ? userProjectMilestoneMap : projectMilestoneMap
            );
            if (!pickedProjectEntry) {
                continue;
            }

            const pickedMilestone = pickRandom(pickedProjectEntry.milestones);
            if (!pickedMilestone) {
                continue;
            }

            const pickedStatus = taskStatuses[0] ?? null;
            if (!pickedStatus) {
                continue;
            }

            const body = JSON.stringify({
                this_week: true,
                milestone_id: pickedMilestone.id,
                estimated_workdays: Math.floor(Math.random() * 10) + 1,
                name: `Task-${user.username}-${i + 1}`,
                status_id: pickedStatus.id,
                description: `Description for ${user.username} task ${i + 1}`,
                progress: 0,
                project_id: pickedProjectEntry.projectId,
                executor_ids: [
                    user.id
                ],
            });

            const res = http.post('http://localhost:5001/tasks', body, {
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${user.token}`,
                    'X-From-Function': 'true',
                    'X-Tenant-Id': user.tenant_id,
                    'X-Tenant-Name': user.tenant_name,
                },
            });

            check(res, {
                'create task status is 200': (r) => r.status === 200,
            });

            if (res.status !== 200) {
                console.error(`Failed to create task for user ${user.username}: ${res.status} ${res.body}`);
                continue;
            }

            const taskBody = typeof res.body === 'string'
                ? (JSON.parse(res.body) as { data?: Task })
                : {};

            if (taskBody.data) {
                tasks.push(taskBody.data);
            }
        }
    }

    return tasks;
}