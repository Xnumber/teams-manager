#include "sql.h"

const char *createPlansSql = R"(
    WITH inserted_plan AS (
        INSERT INTO plans (name, description, start_date, end_date, status)
        VALUES ($1, $2, $3::date, $4::date, COALESCE(NULLIF($5, ''), 'draft')::plan_status)
        RETURNING id, name, description, start_date, end_date, created_at, status
    ),
    insert_projects AS (
        INSERT INTO plans_projects (plan_id, project_id)
        SELECT ip.id, unnest($6::uuid[])
        FROM inserted_plan ip
        WHERE $6::uuid[] IS NOT NULL
    )
    SELECT * FROM inserted_plan
)";