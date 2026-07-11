#include "milestone-reprioritize-sql.h"





const std::string &getMilestoneReprioritizeUpdateTargetSql()
{
    static const std::string sql = R"SQL(
        UPDATE milestones
        SET priority = $1
        WHERE id = $2::uuid
    )SQL";
    return sql;
}

const std::string &getMilestoneReprioritizeReorderSql()
{
    // BEGIN;
    static const std::string sql = R"SQL(
        WITH normalized AS (
            SELECT
                id,
                ROW_NUMBER() OVER (
                    PARTITION BY project_id
                    ORDER BY priority, created_at, id
                ) AS clean_priority
            FROM milestones
            WHERE project_id = $1
        ),
        fixed AS (
            UPDATE milestones m
            SET priority = n.clean_priority
            FROM normalized n
            WHERE m.id = n.id
            RETURNING m.id, m.priority
        ),
        target AS (
            SELECT id, priority
            FROM milestones
            WHERE id = $2
            AND project_id = $1
        ),
        params AS (
            SELECT
                $1::uuid AS project_id,
                $2::uuid AS milestone_id,
                $3::int  AS new_priority,
                (SELECT priority FROM target) AS old_priority
        )
        UPDATE milestones m
        SET priority =
            CASE
                WHEN m.id = (SELECT milestone_id FROM params)
                THEN (SELECT new_priority FROM params)

                WHEN (SELECT new_priority FROM params) < (SELECT old_priority FROM params)
                    AND m.priority >= (SELECT new_priority FROM params)
                    AND m.priority < (SELECT old_priority FROM params)
                THEN m.priority + 1

                WHEN (SELECT new_priority FROM params) > (SELECT old_priority FROM params)
                    AND m.priority <= (SELECT new_priority FROM params)
                    AND m.priority > (SELECT old_priority FROM params)
                THEN m.priority - 1

                ELSE m.priority
            END
        WHERE project_id = $1;
    )SQL";
        // COMMIT;
    return sql;
}
