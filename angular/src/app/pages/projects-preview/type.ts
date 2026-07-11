export interface OrganizationOverviewData {
	teams_count: number;
	projects_count: number;
	users_count: number;
	members_count: number;
	tasks_count: number;
	tasks_estimated_workdays: number | null;
	milestones_count: number;
}

export interface OrganizationOverviewResponse {
	result: 'ok' | 'error';
	data?: OrganizationOverviewData;
	message?: string;
}
