// Tenant creation test data
import { host } from "../const/enviroment";
export const TENANT_NAME = 'TenantForProjectTestABC';
export const TENANT_DESC = 'TenantForProjectTestDescABC';

export const projectCreateData = {
    name: 'TestProject',
    description: 'TestProjectDesc',
};

export const projectCreateData2 = {
    name: 'TestProject2',
    description: 'TestProjectDesc2',
};


export const PROJECT_NAME_UPDATE = 'UpdatedProject';
export const PROJECT_DESCRIPTION_UPDATE = 'UpdatedProjectDesc';

export const getProjectUpdateData = (concurrency_stamp: string) => ({
    name: PROJECT_NAME_UPDATE,
    description: PROJECT_DESCRIPTION_UPDATE,
    concurrency_stamp,
});

export const PROJECT_BASE_URL = `${host}/projects`;
export const PROJECT_SINGLE_URL = (id: string) => `${PROJECT_BASE_URL}/${id}`;