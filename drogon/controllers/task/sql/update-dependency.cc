#include "sql.h"
const char* updateTaskDependencySql = R"SQL(
        WITH pred AS (SELECT $1::uuid AS id),
                 succ AS (SELECT $2::uuid AS id),
        -- try to update existing dependency that has the same predecessor_id
        upd AS (
            UPDATE work_item_dependencies w
            SET successor_type = 'TASK'::work_item_type,
                    successor_id = succ.id,
                    dependency_type = 'FS'::work_item_dependency_type
            FROM pred, succ
            WHERE w.predecessor_id = pred.id
                AND pred.id <> succ.id
            RETURNING w.*
        ),
        -- if no existing row was updated, insert a new one
        ins AS (
            INSERT INTO work_item_dependencies (
                predecessor_type, predecessor_id,
                successor_type, successor_id,
                dependency_type
            )
            SELECT
                'TASK'::work_item_type, pred.id,
                'TASK'::work_item_type, succ.id,
                'FS'::work_item_dependency_type
            FROM pred, succ
            WHERE pred.id <> succ.id
                AND EXISTS (SELECT 1 FROM tasks t WHERE t.id = pred.id)
                AND EXISTS (SELECT 1 FROM tasks t WHERE t.id = succ.id)
                AND NOT EXISTS (
                    SELECT 1 FROM work_item_dependencies w WHERE w.predecessor_id = pred.id
                )
            RETURNING *
        )
        -- return either the updated rows or the inserted row(s)
        SELECT * FROM upd
        UNION ALL
        SELECT * FROM ins;
)SQL";