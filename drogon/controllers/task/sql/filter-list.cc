const char *filterListSql = R"(

WITH filtered_tasks AS (
	SELECT t.*
	FROM tasks t
	WHERE (NULLIF($1, '') IS NULL OR t.team_id = NULLIF($1, '')::uuid)
	  AND (NULLIF($2, '') IS NULL OR t.milestone_id = NULLIF($2, '')::uuid)
	  AND (NULLIF($3, '') IS NULL OR t.project_id = NULLIF($3, '')::uuid)
	  AND (NULLIF($4, '') IS NULL OR t.completed = NULLIF($4, '')::boolean)
	  AND (
		NULLIF($5, '') IS NULL
		OR EXISTS (
			SELECT 1
			FROM executors_tasks et
			WHERE et.task_id = t.id
				AND et.user_id = NULLIF($5, '')::uuid
		)
	  )
),

task_with_users AS (
	SELECT
		t.*,
		COALESCE(mt.mentors, '[]'::json) AS mentors,
		COALESCE(et.executors, '[]'::json) AS executors
	FROM filtered_tasks t
	LEFT JOIN LATERAL (
		SELECT json_agg(DISTINCT jsonb_build_object(
			'task_id', m.task_id,
			'user_id', m.user_id,
			'user_name', m.user_name
		)) AS mentors
		FROM mentors_tasks m
		WHERE m.task_id = t.id
	) mt ON TRUE
	LEFT JOIN LATERAL (
		SELECT json_agg(DISTINCT jsonb_build_object(
			'task_id', e.task_id,
			'user_id', e.user_id,
			'user_name', e.user_name
		)) AS executors
		FROM executors_tasks e
		WHERE e.task_id = t.id
	) et ON TRUE
)
SELECT
	COALESCE(json_agg(to_jsonb(twu) ORDER BY twu.created_at, twu.id), '[]'::json) AS data,
	(SELECT COUNT(*)::int FROM filtered_tasks) AS count
FROM task_with_users twu
)";