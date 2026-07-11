export interface RawTask {
  id: string;
  creator_id: string;
  creator_name: string;
  project_id: string;
  project_name: string;
  task_type_id: string;
  task_type_name: string;
  name: string;
  description: string;
  scheduled_start_date: string; // ISO date string
  scheduled_end_date: string;   // ISO date string
  completion_date?: string;     // ISO date string, optional
  progress: number;
  completed: boolean;
  status: string;
  status_last_changed: string;  // ISO date string
  remark?: string;
  participants?: string;           // 可依實際資料型態調整
  executors?: { task_id: string; user_id: string; user_name: string; }[];           // 可依實際資料型態調整
  mentors?: { task_id: string; user_id: string; user_name: string; }[];           // 可依實際資料型態調整
}









export interface JBTask {
  id: string;
  creator_id: string;
  creator_name: string;
  project_id: string;
  project_name: string;
  task_type_id: string;
  task_type_name: string;
  name: string;
  description: string;
  scheduled_start_date: string; // ISO date string
  scheduled_end_date: string;   // ISO date string
  completion_date?: string;     // ISO date string, optional
  progress: number;
  completed: boolean;
  executor_ids: string[];
  mentor_ids: string[];
  this_week: boolean; // 用於區分工作是屬於 "This Week" 還是 "Next Week"
  status: string;
  status_last_changed: string;  // ISO date string
  remark?: string;
  executors?: string;           // 可依實際資料型態調整
  mentors?: string;           // 可依實際資料型態調整
  milestone_id: string;
  excecutor_time_ratio: number; // 參與者投入時間百分比
}