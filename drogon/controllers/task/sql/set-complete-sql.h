#pragma once
const char* setCompleteSql = R"(
WITH completed_status AS (
	SELECT id, name
	FROM task_status
	WHERE name = '完成'
	LIMIT 1
)
UPDATE tasks AS t
SET
	status_id = cs.id,
	progress = 1.00,
	status_name = cs.name,
	completion_date = CURRENT_DATE,
	completed = true
FROM completed_status AS cs
WHERE t.id = $1;

)";