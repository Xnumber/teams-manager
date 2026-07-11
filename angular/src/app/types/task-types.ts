
export interface TaskType {
	id: string; // UUID
	name: string;
	description?: string;
	concurrency_stamp: string;
	created_at: string; // ISO timestamp
}
