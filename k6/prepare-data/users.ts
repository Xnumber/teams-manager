import http from 'k6/http';
import { check } from 'k6';
import { Team, Tenant } from './type';


type SetupData = {
  tenants?: Tenant[];
  teams: Team[]
};


function pickRandomTenant(tenants: Tenant[]): Tenant {
  const index = Math.floor(Math.random() * tenants.length);
  return tenants[index] ?? {};
}


function getTeamsForTenant(tenantId: string, groupedTeamsByTenantId: Record<string, Team[]>): Team[] {
  return groupedTeamsByTenantId[tenantId] ?? [];
}

function pickRandomTeamId(teams: Team[]): string {
  const index = Math.floor(Math.random() * teams.length);
  return teams[index]?.id ?? '';
}


// { concurrency_stamp: "gen_random_uuid()", created_at: "2026-05-24 14:11:41.046062", email: "k6-user-0-29@example.test", id: "84f9d522-6fd6-472d-945e-a07805be985f", password: "$argon2id$v=19$m=65536,t=2,p=1$NnFNanV5RmNtck13QzJVMg$2Hqk65ONRu4o4pK1l3kT7X6t9IPfkZ+ttZdzIbCatZc", team_id: "5dcfc16b-ad47-4f0a-b1db-308fee4ae81f", team_name: "團隊-1779603098586-40", tenant_id: "78411b20-f6b0-4f0a-a1e9-abb34d5096a3", tenant_name: "Tenant-10", username: "User-0-29" }

export function prepareUsers(data: SetupData): {
  username: string;
  password: string;
  tenant_name: string;
  tenant_id: string;
  team_id: string;
}[] {
  const { teams, tenants } = data;
  const users = [];
  // console.log('Preparing users with tenants:', tenants);
  // console.log('Preparing users with teams:', teams);
  const groupedTeamsByTenantId = teams.reduce((acc, team) => {
    const tenantId = team.tenant_id;
    if (!tenantId) {
      return acc;
    }

    if (!acc[tenantId]) {
      acc[tenantId] = [];
    }
    acc[tenantId].push(team);
    return acc;
  }, {} as Record<string, Team[]>);

  const registerUserUrl = 'http://localhost:5001/register';
  // const tenants = data.tenants ?? [];
  const vuId = __VU;
  const startIndex = 0;
  const endIndex = 30;



  if (tenants?.length === 0) {
    throw new Error('No tenants found, cannot prepare users');
  }

  // console.log(groupedTeamsByTenantId)

  for (let i = startIndex; i < endIndex; i++) {
    const user = {
      username: `User-${vuId}-${i}`,
      email: `k6-user-${vuId}-${i}@example.test`,
      password: '123123',
    };
    const tenant = pickRandomTenant(tenants ?? []);
    const tenantName = tenant.name ?? '';

    if (!tenantName || !tenant.id) {
      throw new Error('Invalid tenant data, cannot prepare users');
    }

    const teams = getTeamsForTenant(tenant.id, groupedTeamsByTenantId);

    if (teams.length === 0) {
      // console.warn(`No teams found for tenant ${tenantName}, skipping user creation for this tenant.`);
      continue;
    }

    const teamId = pickRandomTeamId(teams);
    
    if (!teamId) {
      throw new Error(`Invalid team data for tenant: ${tenantName}`);
    }

    const payload = JSON.stringify({
      tenant_name: tenantName,
      username: user.username,
      email: user.email,
      password: user.password,
      team_id: teamId,
    });

    const params = {
      headers: {
        'Content-Type': 'application/json',
      },
    };
    // console.log(`Registering user: ${user.username} for tenant: ${tenantName} with team ID: ${teamId}`);
    const response = http.post(registerUserUrl, payload, params);
    // console.log(`Response for user ${user.username}:`, res.status, res.body);
    const responseData = JSON.parse(typeof response.body === 'string' ? response.body : '{}');
    // console.log(responseData);
    check(response, {
      'status is 200': (r) => r.status === 200,
    });
    users.push({
      ...responseData.data,
      password: user.password
    });
  }
  return users;
}

export default function (data: SetupData) {
  prepareUsers(data);
}