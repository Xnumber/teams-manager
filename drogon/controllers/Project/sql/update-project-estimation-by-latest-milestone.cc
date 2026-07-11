#include "update-project-estimation-by-latest-milestone.h"
const std::string &updateProjectEstimationDatesByLatestMilestone() {
    static const std::string sql = R"(
            WITH latest_milestone_history AS (
                SELECT milestone_id,
                       complete_date,
                       optimistic_estimated_complete_date,
                       pessimistic_estimated_complete_date,
                       estimated_remaining_workdays
                FROM milestone_estimation_histories
                WHERE project_id = $1::uuid
                ORDER BY estimation_date DESC, complete_date DESC
                LIMIT 1
            ),
            left_milestones_count AS (
                SELECT COUNT(*)::integer AS cnt
                FROM milestones
                WHERE project_id = $1::uuid
                  AND completed = false
            ),
            upserted AS (
                INSERT INTO project_estimation_histories (
                    project_id,
                    estimation_date,
                    complete_date,
                    optimistic_estimated_complete_date,
                    pessimistic_estimated_complete_date,
                    created_at,
                    estimated_remaining_workdays,
                    left_milestones
                )
                SELECT
                    $1::uuid,
                    date_trunc('day', NOW()),
                    lm.complete_date,
                    lm.optimistic_estimated_complete_date,
                    lm.pessimistic_estimated_complete_date,
                    NOW(),
                    lm.estimated_remaining_workdays,
                    lmc.cnt
                FROM latest_milestone_history lm
                CROSS JOIN left_milestones_count lmc
                ON CONFLICT (project_id, estimation_date)
                DO UPDATE SET
                    complete_date = EXCLUDED.complete_date,
                    optimistic_estimated_complete_date = EXCLUDED.optimistic_estimated_complete_date,
                    pessimistic_estimated_complete_date = EXCLUDED.pessimistic_estimated_complete_date,
                    estimated_remaining_workdays = EXCLUDED.estimated_remaining_workdays,
                    left_milestones = EXCLUDED.left_milestones,
                    created_at = NOW()
                RETURNING project_id,
                          complete_date,
                          optimistic_estimated_complete_date,
                          pessimistic_estimated_complete_date
            )
            SELECT u.project_id,
                   u.complete_date AS estimated_end_date,
                   u.optimistic_estimated_complete_date,
                   u.pessimistic_estimated_complete_date
            FROM upserted u
        )";
    return sql;
}