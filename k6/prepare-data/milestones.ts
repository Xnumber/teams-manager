import http from 'k6/http';
import { check } from 'k6';
import { Milestone, Project, UserWithAuthToken } from './type';

/**
 * 為每個使用者在每個專案下創建工作大項，目前每個使用者在每個專案下創建5個工作大項
 * @param users 
 * @param projects 
 */
export function prepareMilestones(users: UserWithAuthToken[], projects: Project[]): any[] {
    const milestones: Milestone[] = [];
    const milestonesPerProject = 5;

    for (const user of users) {
        const userProjects = projects.filter((project) => project.creator_name === user.username);

        for (const project of userProjects) {
            for (let i = 0; i < milestonesPerProject; i++) {
                const body = JSON.stringify({
                    tenant_id: user.tenant_id || project.tenant_id,
                    team_id: user.team_id,
                    name: `Milestone-${project.name}-${i + 1}`,
                    project_id: project.id,
                    description: `Description for ${project.name} milestone ${i + 1}`,
                    tenant_name: user.tenant_name || project.tenant_name,
                });

                const res = http.post('http://localhost:5001/milestones', body, {
                    headers: {
                        'Content-Type': 'application/json',
                        'Authorization': `Bearer ${user.token}`,
                        'X-From-Function': 'true',
                        'X-Tenant-Id': user.tenant_id || project.tenant_id,
                        'X-Tenant-Name': user.tenant_name || project.tenant_name,
                    },
                });

                check(res, {
                    'create milestone status is 200': (r) => r.status === 200,
                });

                if (res.status !== 200) {
                    console.error(`Failed to create milestone for user ${user.username} and project ${project.id}: ${res.status} ${res.body}`);
                    continue;
                }

                const milestoneBody = typeof res.body === 'string'
                    ? (JSON.parse(res.body) as { data?: Milestone })
                    : {};

                if (milestoneBody.data) {
                    milestones.push(milestoneBody.data);
                }
            }
        }
    }

    return milestones;
}