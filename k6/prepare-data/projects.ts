import http from 'k6/http';
import { check } from 'k6';
import { Project, UserWithAuthToken } from './type';


export function prepareProjects(users: UserWithAuthToken[]): Project[] {
    const projectsPerUser = 2;
    const projects: Project[] = [];
    for (const user of users) {
        for (let i = 0; i < projectsPerUser; i++) {
            const body = JSON.stringify({
                name: `Project-${user.username}-${i + 1}`,
                description: `Description for ${user.username} project ${i + 1}`,
                test_count: 0,
                test_pass_count: 0,
                test_auto_count: 0,
                available_developer_count: 0,
                unit_test_coverage: 0,
            });
            const res = http.post('http://localhost:5001/projects', body, {
                headers: {
                    'Content-Type': 'application/json',
                    'Authorization': `Bearer ${user.token}`,
                    'X-From-Function': 'true',
                },
            });
            check(res, {
                'create project status is 200': (r) => r.status === 200,
            });

            if (res.status !== 200) {
                console.error(`Failed to create project for user ${user.username}: ${res.status} ${res.body}`);
            }
            // console.log('Create project response:', res.status, res.body);
            const projectBody = typeof res.body === 'string'
                ? (JSON.parse(res.body) as { data?: Project })
                : {};
            // console.log('Created project:', projectBody);
            if (projectBody.data) {
                projects.push(projectBody.data);
            }
        }
    }
    return projects;
}