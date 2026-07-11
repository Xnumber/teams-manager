// Team type generated from teams table schema
export interface Team {
  id: string;
  tenant_id: string;
  tenant_name: string;
  name: string;
  description?: string;
  concurrency_stamp: string;
  created_at: string; // ISO timestamp
}