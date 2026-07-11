export interface ProjectOverviewData {
	project_id: string;
	remaining_milestones_count: number;
	participating_teams_count: number;
	executors_count: number;
	latest_complete_date: string | null;
	earliest_incomplete_demo_milestone_name: string | null;
	earliest_incomplete_demo_milestone_complete_date: string | null;
}

export interface ProjectOverviewResponse {
	result: string;
	data: ProjectOverviewData;
}
