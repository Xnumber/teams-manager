import { APIRequestContext, test as base, request } from '@playwright/test';
import { baseUrl } from '../const';
import { taskStatusList } from './task-status-generator';

type UserInfo = {
  username: string;
  email: string;
  password: string;
};


const tenantData = {
  name: 'RD2',
  description: 'A tenant for testing'
};


const user_data_1 = {
  username: 'fixture-testuser-1-' + Date.now() + '-' + Math.floor(Math.random() * 10000),
  email: `testuser-1-${Date.now()}@example.com`,
  password: '123456',
  tenant_name: tenantData.name,
};

const user_data_2 = {
  username: 'qqq',
  email: `testuser-2-${Date.now()}@example.com`,
  password: '123456',
  tenant_name: tenantData.name,
};

type TestFixture = {
  apiContext: APIRequestContext
  apiContext2: APIRequestContext;
  authorization1: {
    token: string;
    tenant_name: string;
    message: string;
    tenant_id: string;
  },
  authorization2: {
    token: string;
    tenant_name: string;
    message: string;
    tenant_id: string;
  },
  user_1: {
    tenant_id: string;
    username: string;
    email: string;
    id: string;
  },
  user_2: {
    tenant_id: string;
    username: string;
    email: string;
    id: string;
  },
  tenant: {
    id: string;
    name: string;
    description: string;
  },
  taskStatusList: {
    id: string;
    name: string;
    description: string;
  }[],
  teams: {
    id: string;
    name: string;
    description: string;
    tenant_id: string;
    tenant_name: string;
    concurrency_stamp: string;
    created_at: string;
    updated_at: string;
  }[];
  preparedData: {
    tenant: Record<string, any>;
    teams: Record<string, any>[];
    user_1: Record<string, any>;
    user_2: Record<string, any>;
    authorization1: Record<string, any>;
    authorization2: Record<string, any>;
    taskStatusList: Record<string, any>;
    project: Record<string, any>;
    team: Record<string, any>;
    milestone: Record<string, any>;
    task: Record<string, any>;
  }
}



export const test = base.extend<{}, TestFixture>({
  // tenant_name: [
  //   async ({}, use) => {
  //     // 動態產生唯一 tenant
  //     const tenant_name = 'test-tenant-' + Date.now() + '-' + Math.floor(Math.random() * 10000);
  //     await use(tenant_name);
  //   },
  //   { scope: 'worker' }
  // ],
  tenant: [
    async ({ }, use) => {
      const apiContext = await request.newContext();
      const tenant = await apiContext.post(`${baseUrl}/tenant`, {
        data: tenantData,
        headers: { 'Content-Type': 'application/json' },
      });
      const tenantBody = await tenant.json();
      use(tenantBody.tenant);
    },
    { scope: 'worker' }
  ],
  teams: [
    async ({ tenant }, use) => {
      const apiContext = await request.newContext();
      // 新增一筆 team
      const teamData = {
        name: 'fixture-team-' + Date.now() + '-' + Math.floor(Math.random() * 10000),
        description: 'A team for testing',
        tenant_id: tenant.id
      };
      const createResp = await apiContext.post(`${baseUrl}/teams`, {
        data: teamData,
        headers: { 'Content-Type': 'application/json' }
      });
      const createBody = await createResp.json();

      // 取得 teams 列表
      const listResp = await apiContext.get(`${baseUrl}/teams`);
      const listBody = await listResp.json();
      await use(listBody.data);
    },
    { scope: 'worker' }
  ],
  user_1: [
    async ({ tenant, teams }, use) => {
      // 動態產生唯一 user
      const apiContext = await request.newContext();

      const userRegisterResponse = await apiContext.post(`${baseUrl}/register`, {
        data: {
          ...user_data_1,
          team_id: teams[0].id, // 假設 teams fixture 已經建立了一個 team，並且我們使用它的 id
        }
      });

      const result = await userRegisterResponse.json();
      await use(result.data);
    },
    { scope: 'worker' }
  ],
  user_2: [
    async ({ tenant, teams }, use) => {
      const apiContext = await request.newContext();

      const userRegisterResponse = await apiContext.post(`${baseUrl}/register`, {
        data: {
          ...user_data_2,
          team_id: teams[0].id
        },
      });

      const result = await userRegisterResponse.json();
      // console.log('User Register Response:', result);
      await use(result.data);
    },
    { scope: 'worker' }
  ],
  authorization1: [
    async ({ tenant, user_1 }, use) => {
      // 註冊 user
      const apiContext = await request.newContext();

      // 登入
      const loginResponse = await apiContext.post(`${baseUrl}/login`, {
        data: {
          tenant_name: tenant.name,
          username: user_1.username,
          password: user_data_1.password
        }
      });

      const body = await loginResponse.json();
      // console.log('Login Response Body:', body);
      // console.log('Login Response Body:', body);
      await use(body);
    },
    { scope: 'worker' }
  ],
  authorization2: [
    async ({ tenant, user_2 }, use) => {
      // 註冊 user
      const apiContext = await request.newContext();

      // 登入
      const loginResponse = await apiContext.post(`${baseUrl}/login`, {
        data: {
          tenant_name: tenant.name,
          username: user_2.username,
          password: user_data_2.password
        }
      });

      const body = await loginResponse.json();
      // console.log('Login Response Body:', body);
      // console.log('Login Response Body:', body);
      await use(body);
    },
    { scope: 'worker' }
  ],
  apiContext: [
    async ({ authorization1: authorization }, use) => {
      // console.log('=== Authorization Token in apiContext fixture:', authorization);
      // console.log('Authorization Token in apiContext fixture:', authorization.token);
      const apiContext = await request.newContext({
        extraHTTPHeaders: {
          Authorization: `Bearer ${authorization.token}`,
          'Content-Type': 'application/json'
        },
      });
      await use(apiContext);
    },
    { scope: 'worker' }
  ],
  apiContext2: [
    async ({ authorization2: authorization }, use) => {
      // console.log('=== Authorization Token in apiContext2 fixture:', authorization);
      // console.log('Authorization Token in apiContext2 fixture:', authorization.token);
      const apiContext = await request.newContext({
        extraHTTPHeaders: {
          Authorization: `Bearer ${authorization.token}`,
          'Content-Type': 'application/json'
        },
      });
      await use(apiContext);
    },
    { scope: 'worker' }
  ],
  taskStatusList: [
    async ({ apiContext }, use) => {
      const statuses: taskStatusList[] = ['queued', 'in_progress', 'completed', 'archived'];
      const statusList: { data: { id: string; name: string; description: string }[] } = await apiContext.get(`${baseUrl}/task-status`).then(res => res.json());
      // console.log('Task Status List:', statusList);
      // console.log('Task Status List:', statusList);
      // console.log('Task Status List:', statusList);
      // const getRandomStatus = () => statuses[Math.floor(Math.random() * statuses.length)];
      await use(statusList.data);
    },
    { scope: 'worker' }
  ],
  preparedData: [
    async ({ tenant, teams, user_1, user_2, authorization1, authorization2, taskStatusList, apiContext2 }, use) => {

      const team = await apiContext2.post(`${baseUrl}/teams`, {
        data: {
          tenant_id: tenant.id,
          name: 'Fixture Team',
          description: 'A team for testing',
        }
      }).then(res => res.json());

      const project = await apiContext2.post(`${baseUrl}/projects`, {
        data: {
          available_developer_count: 0,
          description: "0",
          name: "Fixture Project",
          test_auto_count: 0,
          test_count: 0,
          test_pass_count: 0,
          unit_test_coverage: "0"
        }
      }).then(res => res.json());

      const milestone = await apiContext2.post(`${baseUrl}/milestones`, {
        data: {
          description: "Description",
          end_date: "2026-05-28T16:00:00.000Z",
          name: "Milestone for Fixture",
          project_id: project.data.id,
          remark: "備註",
          start_date: "2026-04-30T16:00:00.000Z",
          team_id: team.data.id,
          tenant_id: tenant.id
        }
      }).then(res => res.json());

      const task = await apiContext2.post(`${baseUrl}/tasks`, {
        data: {
          completion_date:"2026-05-01",
          description:"Fixture Task",
          excecutor_time_ratio:0.3,
          executor_ids:[user_1.id, user_2.id],
          milestone_id: milestone.data.id,
          name:"Fixture Task",
          progress : 0.01,
          project_id: project.data.id,
          remark:"備註",
          status_id: taskStatusList[0].id,
          task_type_id: null,
          this_week:true
        }
      }).then(res => res.json());






      const preparedData = {
        tenant,
        teams,
        user_1,
        user_2,
        authorization1,
        authorization2,
        taskStatusList,
        project: project.data,
        team: team.data,
        milestone: milestone.data,
        task: task.data
      };
      await use(preparedData);
    },
    { scope: 'worker' }
  ]
});