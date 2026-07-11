export interface Milestone {
  id: string;
  name: string;
  description?: string;
  project_id?: string;
  project_name?: string;
  team_id?: string;
  team_name?: string;
  tenant_id?: string;
  tenant_name?: string;
  start_date?: string; // ISO date
  end_date?: string; // ISO date
  created_at?: string; // ISO timestamp
  creator_id?: string;
  creator_name?: string;
  concurrency_stamp: string;
  remark?: string;
}
