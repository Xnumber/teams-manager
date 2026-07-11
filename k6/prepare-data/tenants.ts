import http from 'k6/http';
import { check } from 'k6';

const tenants: { name: string; description: string }[] = [];

const numberOfTenants = 10;
// const runId = Date.now();

for (let i = 0; i < numberOfTenants; i++) {
  tenants.push({
    name: `Tenant-${i + 1}`,
    description: `k6 generated tenant ${i + 1}`,
  });
}

export function prepareTenants() {
  const url = 'http://localhost:5001/tenant';
  tenants.forEach((tenant) => {
    const payload = JSON.stringify(tenant);
    const params = {
      headers: {
        'Content-Type': 'application/json',
      },
    };

    const res = http.post(url, payload, params);
    check(res, {
      'status is 200': (r) => r.status === 200,
    });
  });
}

export default function () {
  prepareTenants();
}