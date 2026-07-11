#include "sql.h"
const char *listPlansSql = R"(
    SELECT p.id, p.name, p.description, p.start_date, p.end_date, p.created_at, p.status,
           COALESCE(json_agg(pr.project_id) FILTER (WHERE pr.project_id IS NOT NULL), '[]') AS project_ids
    FROM plans p
    LEFT JOIN plans_projects pr ON p.id = pr.plan_id
    GROUP BY p.id
)";