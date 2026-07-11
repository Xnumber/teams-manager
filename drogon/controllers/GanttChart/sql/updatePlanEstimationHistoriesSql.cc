#include "sql.h"

extern const char *updatePlanEstimationHistoriesSql = R"(
    INSERT INTO plan_estimation_histories (
        plan_id,
        estimation_date,
        complete_date,
        optimistic_estimated_complete_date,
        pessimistic_estimated_complete_date,
        estimated_remaining_workdays,
        created_at
    ) VALUES (
        $1,
        $2,
        $3,
        $4,
        $5,
        $6,
        NOW()
    )
    ON CONFLICT (plan_id, estimation_date) DO UPDATE SET
        complete_date = EXCLUDED.complete_date,
        optimistic_estimated_complete_date = EXCLUDED.optimistic_estimated_complete_date,
        pessimistic_estimated_complete_date = EXCLUDED.pessimistic_estimated_complete_date,
        estimated_remaining_workdays = EXCLUDED.estimated_remaining_workdays
)";