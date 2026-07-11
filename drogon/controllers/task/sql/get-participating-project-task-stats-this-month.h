#pragma once
const char *getParticipatingProjectTaskStatsThisMonthSql = R"(
WITH month_tasks AS (
	SELECT
		t.id,
		t.project_id
	FROM tasks t
	WHERE t.created_at >= date_trunc('month', CURRENT_DATE)
	  AND t.created_at < (date_trunc('month', CURRENT_DATE) + interval '1 month')
	  AND t.project_id IS NOT NULL
	  AND (
			EXISTS (
				SELECT 1
				FROM mentors_tasks mt
				WHERE mt.task_id = t.id
				  AND mt.user_id = $1::uuid
			)
			OR EXISTS (
				SELECT 1
				FROM executors_tasks et
				WHERE et.task_id = t.id
				  AND et.user_id = $1::uuid
			)
	  )
)
SELECT
	p.id AS project_id,
	p.name AS project_name,
	COUNT(DISTINCT mt.id) AS task_count
FROM month_tasks mt
JOIN projects p ON p.id = mt.project_id
GROUP BY p.id, p.name
ORDER BY task_count DESC, p.name ASC
)";
