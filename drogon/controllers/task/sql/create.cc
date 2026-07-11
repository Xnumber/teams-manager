// 更新task的時候，更新該project_id的metrics history。
#include "sql.h"

const char *createTasksSql = R"(
    WITH project_info AS (
        SELECT
            p.id,
            p.name AS project_name
        FROM projects p
        WHERE p.id = $1
    ),
    creator_info AS (
        SELECT
            u.id,
            u.username
        FROM users u
        WHERE u.id = $4::uuid
    ),
    tenant_info AS (
        SELECT
            t.id,
            t.name
        FROM tenants t
        WHERE t.id = $5::uuid
    ),
    milestone_info AS (
        SELECT
            m.id,
            m.name AS milestone_name
        FROM milestones m
        WHERE m.id = $2
    ),
    team_info AS (
        SELECT
            t.id,
            t.name AS team_name
        FROM teams t
        WHERE t.id = $3::uuid
    ),

    new_task AS (
        INSERT INTO tasks (
            creator_id,
            creator_name,

            tenant_id,
            tenant_name,

            project_id,
            project_name,

            milestone_id,
            milestone_name,

            team_id,
            team_name,

            task_type_id,
            task_type_name,

            name,
            description,

            scheduled_start_date,
            scheduled_end_date,

            status_id,
            status_name,

            estimated_workdays,
            priority,

            progress,
            completed,
            delayed
        )
        SELECT
            $4,     -- creator_id
            (select username from creator_info),     -- creator_name
            $5,     -- tenant_id
            (select name from tenant_info),     -- tenant_name

            p.id,
            p.project_name,

            m.id,
            m.milestone_name,

            t.id,
            t.team_name,

            CASE
                WHEN $6 = '' THEN NULL
                ELSE $6::uuid
            END,     -- task_type_id
            $7,     -- task_type_name

            $8,    -- task_name
            $9,    -- description

            NULLIF($10, '')::date,    -- scheduled_start_date
            NULLIF($11, '')::date,    -- scheduled_end_date

            $12,    -- status_id
            $13,    -- status_name

            $14,    -- estimated_workdays
            COALESCE((SELECT MAX(priority) FROM tasks WHERE milestone_id = m.id) + 1, 1),    -- priority

            0,
            false,
            false
        FROM project_info p
        CROSS JOIN milestone_info m
        CROSS JOIN team_info t
        RETURNING id, $8 AS name, $9 AS description, $10 AS scheduled_start_date, $11 AS scheduled_end_date, $12 AS status_id, $13 AS status_name, $14 AS estimated_workdays, priority
    ),

    insert_executors AS (
        INSERT INTO executors_tasks (
            task_id,
            user_id,
            user_name
        )
        SELECT
            nt.id,
            u.id,
            u.username
        FROM new_task nt
        CROSS JOIN unnest($15::uuid[]) AS executor_id
        JOIN users u
            ON u.id = executor_id
    ),

    insert_mentors AS (
        INSERT INTO mentors_tasks (
            task_id,
            user_id,
            user_name
        )
        SELECT
            nt.id,
            u.id,
            u.username
        FROM new_task nt
        CROSS JOIN unnest($16::uuid[]) AS mentor_id
        JOIN users u
            ON u.id = mentor_id
    ),

    parsed_dependencies AS (
        SELECT
            nt.id as task_id,
            (dep->>'predecessorType')::work_item_type as pred_type,
            (dep->>'predecessorId')::uuid as pred_id,
            COALESCE((dep->>'dependencyType'), 'FS')::work_item_dependency_type as dep_type
        FROM new_task nt,
             jsonb_array_elements(COALESCE($17::jsonb, '[]'::jsonb)) AS dep
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
            task_id,
            dep_type
        FROM parsed_dependencies
    ),

    task_count AS (
        select count(*) from tasks 
        where project_id = $1
            and completed = false
    ),


    
    update_metrics AS (
        INSERT INTO project_metrics_history (
            project_id,
            date,
            task_count,
            task_added_count,
            task_completed_count,
            milestone_count,
            milestone_added_count,
            milestone_completed_count,
            changed_at
        )
        VALUES (
            $1,     -- project_id
            CURRENT_DATE,
            (SELECT count FROM task_count),
            1,
            0,
            0,
            0,
            0,
            NOW()
        )
        ON CONFLICT (project_id,date)
        DO UPDATE
        SET
            task_count = (SELECT count FROM task_count),
            task_added_count = project_metrics_history.task_added_count + 1,
            changed_at = NOW()
    )

    SELECT id, name
    FROM new_task;
)";