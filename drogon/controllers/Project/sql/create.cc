#include "sql.h"




const char *createProjectSql = R"(
    WITH creator_info AS (
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
        WHERE t.id = $1::uuid
    ),
    new_project AS (
        INSERT INTO projects (
            creator_id,
            creator_name,
            tenant_id,
            tenant_name,
            name,
            description,
            concurrency_stamp,
            estimated_remaining_workdays,
            priority
        )
        SELECT
            $4,     -- creator_id
            (select username from creator_info),     -- creator_name
            $1,     -- tenant_id
            (select name from tenant_info),     -- tenant_name
            $2,     -- name
            NULLIF($3, ''),     -- description
            $5,     -- concurrency_stamp
            COALESCE($6::integer, 0),     -- estimated_remaining_workdays
            COALESCE((SELECT MAX(priority) FROM projects) + 1, 1)
        RETURNING *
    ),
    parsed_dependencies AS (
        SELECT
            np.id as project_id,
            (dep->>'predecessorType')::work_item_type as pred_type,
            (dep->>'predecessorId')::uuid as pred_id,
            COALESCE((dep->>'dependencyType'), 'FS')::work_item_dependency_type as dep_type
        FROM new_project np,
             jsonb_array_elements(COALESCE($7::jsonb, '[]'::jsonb)) AS dep
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
            'PROJECT'::work_item_type,
            project_id,
            dep_type
        FROM parsed_dependencies
    )
    SELECT * FROM new_project;
)";