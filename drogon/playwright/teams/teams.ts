// Team creation test data
import { host } from "../const/enviroment";
export const TENANT_NAME = 'TenantForTeamsTest';
export const TENANT_DESC = 'TenantForTeamsTestDesc';

export const teamCreateData = {
    name: 'TestTeam',
    description: 'TestTeamDesc',
};

export const TEAM_NAME_UPDATE = 'UpdatedTeam';
export const TEAM_DESCRIPTION_UPDATE = 'UpdatedTeamDesc';

export const getTeamUpdateData = (concurrency_stamp: string) => ({
    name: TEAM_NAME_UPDATE,
    description: TEAM_DESCRIPTION_UPDATE,
    concurrency_stamp,
});

export const TEAM_BASE_URL = `${host}/teams`;
export const TEAM_SINGLE_URL = (id: string) => `${TEAM_BASE_URL}/${id}`;
