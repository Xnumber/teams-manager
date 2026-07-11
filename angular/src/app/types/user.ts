
export interface User {
	id: string; // UUID
	tenant_id: string; // UUID
	username: string;
	email: string;
	password: string;
	concurrency_stamp: string;
	created_at: string; // ISO timestamp
}
