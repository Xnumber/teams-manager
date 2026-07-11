
export interface Project {
	id: string;
	tenant_id: string;
	name: string;
	description?: string;
	test_count: number;
	test_pass_count: number;
	test_auto_count: number;
	available_developer_count: number;
	unit_test_coverage: number;
	concurrency_stamp: string;
	created_at: string; // ISO timestamp
	tasks_count_in_queue: number;
	milestones_count_in_queue: number;
}


export interface UserProjectSettings {
	user_id: string;
	project_id: string;
	executor_time_ratio: number; // 0 to 1
}