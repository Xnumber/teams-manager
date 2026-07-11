#include "sql.h"

const char *updatePlansSql = R"(
    WITH new_projects AS (
        SELECT unnest($2::uuid[]) AS project_id
        WHERE $2::uuid[] IS NOT NULL
    ),
    delete_removed_projects AS (
        DELETE FROM plans_projects
        WHERE plan_id = $1
            AND $2::uuid[] IS NOT NULL
            AND project_id NOT IN (SELECT project_id FROM new_projects)
    ),
    insert_new_projects AS (
        INSERT INTO plans_projects (plan_id, project_id)
        SELECT $1::uuid, project_id FROM new_projects
        ON CONFLICT DO NOTHING
    )
    UPDATE plans
    SET
        name = COALESCE(NULLIF($3, ''), name),
        description = COALESCE(NULLIF($4, ''), description),
        start_date = COALESCE(NULLIF($5, '')::date, start_date),
        end_date = COALESCE(NULLIF($6, '')::date, end_date),
        status = COALESCE(NULLIF($7, '')::plan_status, status)
        WHERE id = $1
    RETURNING id, name, description, start_date, end_date, created_at, status
)";