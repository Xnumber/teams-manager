import { host } from "../const/enviroment";

export const taskTypeCreateData = {
	name: 'TestTaskType',
	description: 'TestTaskTypeDesc',
};
// Update data object
export const getTaskTypeUpdateData = (concurrency_stamp: string) => ({
	name: TASK_TYPE_NAME_UPDATE,
	description: TASK_TYPE_DESCRIPTION_UPDATE,
	concurrency_stamp,
});

// API endpoints
export const TASK_TYPE_BASE_URL = `${host}/task-types`;
export const TASK_TYPE_SINGLE_URL = (id: string) => `${TASK_TYPE_BASE_URL}/${id}`;

// Update variables
export const TASK_TYPE_NAME_UPDATE = 'UpdatedTaskType';
export const TASK_TYPE_DESCRIPTION_UPDATE = 'UpdatedTaskTypeDesc';