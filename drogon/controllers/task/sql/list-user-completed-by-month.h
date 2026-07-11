#pragma once
// 先用 created_at 來過濾。如果未來有需要，也可以改成 completion_date。
inline const char* listUserCompletedTasksByMonthSql = R"(
SELECT t.*
FROM tasks t
LEFT JOIN executors_tasks et ON t.id = et.task_id
LEFT JOIN mentors_tasks mt ON t.id = mt.task_id
WHERE t.completed = true
  AND (
    et.user_id = $1
    OR mt.user_id = $1
  )
  AND t.created_at >= date_trunc('month', ($2 || '-01')::date)
  AND t.created_at < (date_trunc('month', ($2 || '-01')::date) + interval '1 month')
)";