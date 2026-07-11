#include "reprioritize-sql.h"
const std::string &getReprioritizeUpdateTargetSql()
{
    static const std::string sql = R"SQL(
        UPDATE projects
        SET priority = $1
        WHERE id = $2::uuid
    )SQL";
    return sql;
}
const std::string &getReprioritizeReorderSql()
{
    static const std::string sql = R"SQL(
        WITH direction AS (
            SELECT (
                SELECT COUNT(*)
                FROM projects p2
                WHERE p2.priority <> 0
                  AND p2.priority < $2
            ) < GREATEST($2 - 1, 0) AS moved_down
        ),
        ranked AS (
            SELECT
                p.id,
                ROW_NUMBER() OVER (
                    ORDER BY
                        CASE
                            WHEN $2 = 0 THEN p.priority::numeric
                            WHEN p.id = $1::uuid AND d.moved_down THEN ($2::numeric + 0.5)
                            WHEN p.id = $1::uuid THEN ($2::numeric - 0.5)
                            WHEN d.moved_down AND p.priority > $2 THEN (p.priority + 1)::numeric
                            WHEN NOT d.moved_down AND p.priority >= $2 THEN (p.priority + 1)::numeric
                            ELSE p.priority::numeric
                        END,
                        p.created_at,
                        p.id
                ) AS new_priority
            FROM projects p
            CROSS JOIN direction d
                        WHERE p.priority <> 0
        )
        UPDATE projects p
        SET priority = ranked.new_priority
        FROM ranked
        WHERE p.id = ranked.id
    )SQL";
    return sql;
}