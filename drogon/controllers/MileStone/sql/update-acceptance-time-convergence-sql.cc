#include "update-acceptance-time-convergence-sql.h"

const std::string &getUpdateAcceptanceTimeConvergenceSql()
{
    static const std::string sql = R"SQL(
                WITH milestone_ctx AS (
                    SELECT m.project_id
                    FROM milestones m
                    WHERE m.id = $1::uuid
                ),
                left_tasks_count AS (
                    SELECT COUNT(*)::integer AS cnt
                    FROM tasks t
                    WHERE t.milestone_id = $1::uuid
                      AND t.completed = false
                ),
                upserted AS (
                    INSERT INTO milestone_estimation_histories
                        (milestone_id, project_id, estimation_date, complete_date,
                         optimistic_estimated_complete_date, pessimistic_estimated_complete_date, created_at,
                         left_tasks)
                    SELECT $1,
                           mc.project_id,
                           date_trunc('day', NOW()),
                           $2::timestamp,
                           $3::timestamp,
                           $4::timestamp,
                           NOW(),
                           ltc.cnt
                    FROM milestone_ctx mc
                    CROSS JOIN left_tasks_count ltc
                    ON CONFLICT (milestone_id, estimation_date)
                    DO UPDATE SET
                        project_id = EXCLUDED.project_id,
                        complete_date = EXCLUDED.complete_date,
                        optimistic_estimated_complete_date = EXCLUDED.optimistic_estimated_complete_date,
                        pessimistic_estimated_complete_date = EXCLUDED.pessimistic_estimated_complete_date,
                        left_tasks = EXCLUDED.left_tasks,
                        created_at = NOW()
                    RETURNING milestone_id, complete_date
                ),
                updated_self AS (
                    UPDATE milestones m
                    SET estimated_end_date = $2::date
                    WHERE m.id = $1::uuid
                    RETURNING m.id
                )
                UPDATE milestones m
                SET estimated_start_date = i.complete_date
                                FROM upserted i
                WHERE m.dependency_milestone_id IS NOT NULL
                  AND m.dependency_milestone_id = i.milestone_id
    )SQL";
    return sql;
}