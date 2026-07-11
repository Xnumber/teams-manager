export type Tenant = {
    id?: string;
    name?: string;
};

export type Team = {
    "id": string,
    "name": string,
    "tenant_id": string,
    "tenant_name": string
}

export type UserWithAuthToken = {
    id: string;
    username: string;
    token: string;
    team_id: string;
    tenant_id: string;
    tenant_name: string;
}

export type Project = {
    available_developer_count: number;
    concurrency_stamp: string;
    created_at: string;
    creator_id: string;
    creator_name: string;
    description: string;
    estimated_remaining_workdays: number;
    id: string;
    name: string;
    priority: number;
    tenant_id: string;
    tenant_name: string;
    test_auto_count: number;
    test_count: number;
    test_pass_count: number;
    unit_test_coverage: string;
};

export type Milestone = {
    id: string;
    name: string;
    description: string;
    project_id: string;
    team_id: string;
    tenant_id: string;
};