import http from 'k6/http';
import { check } from 'k6';

type Tenant = {
  id?: string;
};

const teams: { name: string; description: string }[] = [];

const numberOfTeams = 100;
const runId = Date.now();

for (let i = 0; i < numberOfTeams; i++) {
  teams.push({
    name: `團隊-${runId}-${i + 1}`,
    description: `團隊 ${i + 1}`,
  });
}

function getTenants(): Tenant[] {
  const tenantListUrl = 'http://localhost:5001/tenant';
  const res = http.get(tenantListUrl);
  const ok = check(res, {
    'tenant list status is 200': (r) => r.status === 200,
  });

  if (!ok) {
    return [];
  }

  const body = res.json() as {
    tenants?: Tenant[];
  };

  if (Array.isArray(body?.tenants) && body.tenants.length > 0) {
    return body.tenants;
  }

  return [];
}

function pickRandomTenantId(tenants: Tenant[]): string {
  const index = Math.floor(Math.random() * tenants.length);
  return tenants[index]?.id ?? '';
}

export function prepareTeams() {
  const createTeamUrl = 'http://localhost:5001/teams';
  const tenants = getTenants();

  if (tenants.length === 0) {
    throw new Error('No tenants found, cannot prepare teams');
  }

  teams.forEach((team) => {
    console.log('Preparing team:', team.name);
    const tenantId = pickRandomTenantId(tenants);
    const payload = JSON.stringify({
      ...team,
      tenant_id: tenantId,
    });
    const params = {
      headers: {
        'Content-Type': 'application/json',
      },
    };

    const res = http.post(createTeamUrl, payload, params);
    check(res, {
      'status is 200': (r) => r.status === 200,
    });
  });
}

export default function () {
  prepareTeams();
}