#include "sql.h"
const char *updatePlanMetricsSql = R"SQL(
WITH related_projects AS (
	SELECT pp.project_id
	FROM plans_projects pp
	WHERE pp.plan_id = $1::uuid
),
aggregated AS (
	SELECT
		$1::uuid AS plan_id,
		CURRENT_DATE AS date,
		(
			SELECT COUNT(*)::int
			FROM tasks t
			WHERE t.project_id IN (SELECT project_id FROM related_projects)
		) AS task_count,
		(
			SELECT COUNT(*)::int
			FROM tasks t
			WHERE t.project_id IN (SELECT project_id FROM related_projects)
			  AND t.created_at::date = CURRENT_DATE
		) AS task_added_count,
		(
			SELECT COUNT(*)::int
			FROM tasks t
			WHERE t.project_id IN (SELECT project_id FROM related_projects)
			  AND t.completed = true
			  AND t.completion_date = CURRENT_DATE
		) AS task_completed_count,
		(
			SELECT COUNT(*)::int
			FROM milestones m
			WHERE m.project_id IN (SELECT project_id FROM related_projects)
		) AS milestone_count,
		(
			SELECT COUNT(*)::int
			FROM milestones m
			WHERE m.project_id IN (SELECT project_id FROM related_projects)
			  AND m.created_at::date = CURRENT_DATE
		) AS milestone_added_count,
		(
			SELECT COUNT(*)::int
			FROM milestones m
			WHERE m.project_id IN (SELECT project_id FROM related_projects)
			  AND m.completed = true
			  AND m.completion_date = CURRENT_DATE
		) AS milestone_completed_count,
		NOW() AS changed_at
)
INSERT INTO plan_metrics_history (
	plan_id,
	date,
	task_count,
	task_added_count,
	task_completed_count,
	milestone_count,
	milestone_added_count,
	milestone_completed_count,
	changed_at
)
SELECT
	a.plan_id,
	a.date,
	a.task_count,
	a.task_added_count,
	a.task_completed_count,
	a.milestone_count,
	a.milestone_added_count,
	a.milestone_completed_count,
	a.changed_at
FROM aggregated a
ON CONFLICT (plan_id, date)
DO UPDATE SET
	task_count = EXCLUDED.task_count,
	task_added_count = EXCLUDED.task_added_count,
	task_completed_count = EXCLUDED.task_completed_count,
	milestone_count = EXCLUDED.milestone_count,
	milestone_added_count = EXCLUDED.milestone_added_count,
	milestone_completed_count = EXCLUDED.milestone_completed_count,
	changed_at = EXCLUDED.changed_at
RETURNING
	plan_id,
	date,
	task_count,
	task_added_count,
	task_completed_count,
	milestone_count,
	milestone_added_count,
	milestone_completed_count,
	changed_at;
)SQL";