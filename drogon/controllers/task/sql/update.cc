// 更新task，包括关联的executors/mentors/dependencies。不更新metrics。
#include "sql.h"

const char *updateTasksSql = R"(
    WITH task_update AS (
        UPDATE tasks
        SET
            creator_id = $3::uuid,
            creator_name = (SELECT username FROM users WHERE id = $3::uuid),
            tenant_id = $4::uuid,
            tenant_name = (SELECT name FROM tenants WHERE id = $4::uuid),
            project_id = $5::uuid,
            project_name = (SELECT name FROM projects WHERE id = $5::uuid),
            milestone_id = CASE WHEN $6 = '' THEN NULL ELSE $6::uuid END,
            milestone_name = (SELECT name FROM milestones WHERE id = CASE WHEN $6 = '' THEN NULL ELSE $6::uuid END),
            team_id = $7::uuid,
            team_name = (SELECT name FROM teams WHERE id = $7::uuid),
            task_type_id = CASE WHEN $8 = '' THEN NULL ELSE $8::uuid END,
            task_type_name = $9,
            name = $10,
            description = $11,
            scheduled_start_date = NULLIF($12, '')::date,
            scheduled_end_date = NULLIF($13, '')::date,
            status_id = $14::uuid,
            status_name = $15,
            estimated_workdays = $16,
            -- priority = $17,
            concurrency_stamp = gen_random_uuid()::character varying
        WHERE
            id = $1::uuid
            AND concurrency_stamp = $2
        RETURNING id, name
    ),
    delete_executors AS (
        DELETE FROM executors_tasks
        WHERE task_id = $1::uuid
        RETURNING task_id
    ),
    delete_mentors AS (
        DELETE FROM mentors_tasks
        WHERE task_id = $1::uuid
        RETURNING task_id
    ),
    delete_dependencies AS (
        DELETE FROM work_item_dependencies
        WHERE successor_id = $1::uuid AND successor_type = 'TASK'::work_item_type
        RETURNING successor_id
    ),
    parsed_dependencies AS (
        SELECT
            (dep->>'predecessorType')::work_item_type as pred_type,
            (dep->>'predecessorId')::uuid as pred_id,
            COALESCE((dep->>'dependencyType'), 'FS')::work_item_dependency_type as dep_type
        FROM jsonb_array_elements(COALESCE($19::jsonb, '[]'::jsonb)) AS dep
    ),
    insert_executors AS (
        INSERT INTO executors_tasks (
            task_id,
            user_id,
            user_name
        )
        SELECT
            $1::uuid,
            u.id,
            u.username
        FROM unnest($17::uuid[]) AS executor_id
        JOIN users u ON u.id = executor_id
        RETURNING task_id
    ),
    insert_mentors AS (
        INSERT INTO mentors_tasks (
            task_id,
            user_id,
            user_name
        )
        SELECT
            $1::uuid,
            u.id,
            u.username
        FROM unnest($18::uuid[]) AS mentor_id
        JOIN users u ON u.id = mentor_id
        RETURNING task_id
    ),
    insert_dependencies AS (
        INSERT INTO work_item_dependencies (
            predecessor_type,
            predecessor_id,
            successor_type,
            successor_id,
            dependency_type
        )
        SELECT
            pred_type,
            pred_id,
            'TASK'::work_item_type,
            $1::uuid,
            dep_type
        FROM parsed_dependencies
        RETURNING successor_id
    )
    SELECT tu.id, tu.name
    FROM task_update tu
    LEFT JOIN delete_executors de ON true
    LEFT JOIN delete_mentors dm ON true
    LEFT JOIN delete_dependencies dd ON true
    LEFT JOIN insert_executors ie ON true
    LEFT JOIN insert_mentors im ON true
    LEFT JOIN insert_dependencies id ON true;
)";