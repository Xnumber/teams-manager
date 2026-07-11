import http from 'k6/http';
import { check } from 'k6';
import { prepareTeams } from "./prepare-data/teams.ts";
import tenants, { prepareTenants } from "./prepare-data/tenants.ts";
import { prepareUsers } from "./prepare-data/users.ts";
import { prepareProjects } from "./prepare-data/projects.ts";
import { prepareMilestones } from './prepare-data/milestones.ts';
import { Team, Tenant, UserWithAuthToken } from './prepare-data/type.ts';
import { prepareTasks } from './prepare-data/tasks.ts';



function getTenants(): Tenant[] {
    const res = http.get('http://localhost:5001/tenant');
    const tenantsOk = check(res, {
        'tenant list status is 200': (r) => r.status === 200,
    });

    if (!tenantsOk) {
        throw new Error('Failed to preload tenants');
    }

    const tenantsBody = res.json() as {
        tenants?: Tenant[];
    };

    return Array.isArray(tenantsBody?.tenants) ? tenantsBody.tenants : [];
}

function getTeams(): Team[] {
    const res = http.get('http://localhost:5001/teams');
    const teamsOk = check(res, {
        'teams list status is 200': (r) => r.status === 200,
    });

    if (!teamsOk) {
        throw new Error('Failed to preload teams');
    }
    
    const teamsBody = res.json() as {
        data?: Team[];
    };
    // console.log('Fetched teams:', teamsBody.data);

    return Array.isArray(teamsBody.data) ? teamsBody.data : [];
}



function getUserWithAuthTokens(users: { username: string; password: string; tenant_name: string; tenant_id: string; team_id: string }[]): UserWithAuthToken[] {
    
    const tokens: UserWithAuthToken[] = [];
    // 依據users的長度，隨機選取其中的10個使用者來進行登入，獲取token
    const randomPickedUsers = users.sort(() => 0.5 - Math.random()).slice(0, 10);


    randomPickedUsers.forEach(user => {
        // console.log(user)
        const body  = JSON.stringify({
            tenant_name: user.tenant_name,
            username: user.username,
            password: user.password,
        });
        // console.log('Login payload:', body);
        const res = http.post('http://localhost:5001/login', body, {
            headers: {
                'Content-Type': 'application/json',
            },
        });

        const loginOk = check(res, {
            'login status is 200': (r) => r.status === 200,
        });

        if (!loginOk) {
            console.error(`Failed to log in user ${user.username}`);
            return;
        }

        const resBody = res.json() as {
            token: string;
            user_id: string;
        };



        

        if (resBody.token) {
            tokens.push({
                username: user.username,
                token: resBody.token,
                tenant_id: user.tenant_id,
                team_id: user.team_id,
                tenant_name: user.tenant_name,
                id: resBody.user_id
            });
        } else {
            console.error(`No token received for user ${user.username}`);
        }
    });

    return tokens;
}



export function setup() {
    prepareTenants();
    prepareTeams();

    const tenants = getTenants();
    const teams = getTeams();

    const users = prepareUsers({ tenants, teams });
    
    const userTokens = getUserWithAuthTokens(users);
    
    const projects = prepareProjects(userTokens);
    const milestones = prepareMilestones(userTokens, projects);
    
    
    const tasks = prepareTasks(userTokens, projects, milestones);
    console.log('======Seed summary======');
    console.log('Tenants created:', tenants.length);
    console.log('Teams created:', teams.length);
    console.log('Users created:', users.length);
    console.log('Projects created:', projects.length);
    console.log('Milestones created:', milestones.length);
    console.log('Tasks created:', tasks.length);
    console.log('======Seed summary======');

    // console.log('User tokens:', userTokens);


    return {
        tenants,
        teams
    };
}


// export const options: Options = {
//     scenarios: {
//         users: {
//             executor: 'per-vu-iterations',
//             exec: 'prepareUsers',
//             vus: 10,
//             iterations: 1,
//             // executor: 'constant-arrival-rate',
//             // rate: 100,
//             // timeUnit: '1s',
//             // duration: '1s',
//             // preAllocatedVUs: 100,
//             // maxVUs: 200,
//         },
//     },
// };



export default function () { }
// export { prepareUsers };