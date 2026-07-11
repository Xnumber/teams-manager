export interface User {
  id: string;
  tenant_id: string;
  username: string;
  email: string;
  password?: string;
  concurrency_stamp?: string;
  created_at?: string;
}

export interface ApiResponse<T> {
  result: string;
  data: T;
  count?: number;
  message?: string;
}
