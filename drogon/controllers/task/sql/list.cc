#pragma once
const char* listSql = R"(
    SELECT
        t.*,
        COALESCE(
            json_agg(DISTINCT jsonb_build_object(
                'task_id', mt.task_id,
                'user_id', mt.user_id,
                'user_name', mt.user_name
            )) FILTER (WHERE mt.user_id IS NOT NULL),
            '[]'
        ) AS mentors,
        COALESCE(
            json_agg(DISTINCT jsonb_build_object(
                'task_id', et.task_id,
                'user_id', et.user_id,
                'user_name', et.user_name
            )) FILTER (WHERE et.user_id IS NOT NULL),
            '[]'
        ) AS executors
    FROM tasks t
    LEFT JOIN mentors_tasks mt ON t.id = mt.task_id
    LEFT JOIN executors_tasks et ON t.id = et.task_id
    WHERE (
        NULLIF($1, '') IS NULL
        OR t.name ILIKE '%' || $1 || '%'
        OR COALESCE(t.description, '') ILIKE '%' || $1 || '%'
    )
    GROUP BY t.id
)";