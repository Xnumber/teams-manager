#include "reprioritize-sql.h"










const std::string &getTaskReprioritizeUpdateTargetSql()
{
    static const std::string sql = R"SQL(
        UPDATE tasks
        SET priority = $1
        WHERE id = $2::uuid AND tenant_id = $3::uuid
    )SQL";
    return sql;
}

const std::string &getTaskReprioritizeReorderSql()
{
    static const std::string sql = R"SQL(
        WITH normalized AS (
            SELECT
                id,
                ROW_NUMBER() OVER (
                    PARTITION BY milestone_id
                    ORDER BY priority, created_at, id
                ) AS clean_priority
            FROM tasks
            WHERE milestone_id = $1
        ),
        fixed AS (
            UPDATE tasks t
            SET priority = n.clean_priority
            FROM normalized n
            WHERE t.id = n.id
            RETURNING t.id, t.priority
        ),
        target AS (
            SELECT id, priority
            FROM tasks
            WHERE id = $2
            AND milestone_id = $1
        ),
        params AS (
            SELECT
                $1::uuid AS milestone_id,
                $2::uuid AS task_id,
                $3::int  AS new_priority,
                (SELECT priority FROM target) AS old_priority
        )
        UPDATE tasks t
        SET priority =
            CASE
                WHEN t.id = (SELECT task_id FROM params)
                THEN (SELECT new_priority FROM params)

                WHEN (SELECT new_priority FROM params) < (SELECT old_priority FROM params)
                    AND t.priority >= (SELECT new_priority FROM params)
                    AND t.priority < (SELECT old_priority FROM params)
                THEN t.priority + 1

                WHEN (SELECT new_priority FROM params) > (SELECT old_priority FROM params)
                    AND t.priority <= (SELECT new_priority FROM params)
                    AND t.priority > (SELECT old_priority FROM params)
                THEN t.priority - 1

                ELSE t.priority
            END
        WHERE milestone_id = $1;
    )SQL";
    
    return sql;
}