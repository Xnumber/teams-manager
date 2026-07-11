const char *historyTasksSql = R"(
WITH user_tasks AS (
    SELECT task_id
    FROM executors_tasks
    WHERE user_id = $1

    UNION

    SELECT task_id
    FROM mentors_tasks
    WHERE user_id = $1
)
SELECT
    t.id,
    t.name,
    t.project_id,
    t.project_name,
    t.milestone_id,
    t.description,
    t.status_id,
    COALESCE(
        json_agg(
            DISTINCT jsonb_build_object(
                'task_id', mt.task_id,
                'user_id', mt.user_id,
                'user_name', mt.user_name
            )
        ) FILTER (WHERE mt.user_id IS NOT NULL),
        '[]'
    ) AS mentors,

    COALESCE(
        json_agg(
            DISTINCT jsonb_build_object(
                'task_id', et.task_id,
                'user_id', et.user_id,
                'user_name', et.user_name
            )
        ) FILTER (WHERE et.user_id IS NOT NULL),
        '[]'
    ) AS executors

FROM users u

JOIN user_tasks ut
    ON TRUE

JOIN tasks t
    ON t.id = ut.task_id

LEFT JOIN mentors_tasks mt
    ON mt.task_id = t.id

LEFT JOIN executors_tasks et
    ON et.task_id = t.id

WHERE u.id = $1
    AND t.scheduled_end_date < $2
    AND t.description ILIKE '%' || COALESCE($4, '') || '%'
    AND t.name ILIKE '%' || COALESCE($3, '') || '%'
    AND (
        $5::uuid IS NULL
        OR t.milestone_id = $5::uuid
    )
    AND t.project_name ILIKE '%' || COALESCE($6, '') || '%'
    AND (
        $7::uuid IS NULL
        OR t.status_id = $7::uuid
    )

GROUP BY
    u.id,
    u.username,
    t.id,
    t.name

ORDER BY t.scheduled_completion_date DESC
)";