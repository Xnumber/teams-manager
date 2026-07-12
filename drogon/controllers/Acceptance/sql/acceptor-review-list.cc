#include "sql.h"

const char* acceptorReviewAcceptanceListRequestSql = R"SQL(
    SELECT
        tar.id,
        tar.task_id,
        tar.requester_id,
        tar.created_at
    FROM task_acceptance_requests tar
    LEFT JOIN task_acceptor tac ON tac.task_id = tar.task_id
    WHERE ($1 IS NULL OR tar.requester_id = $1)
    GROUP BY tar.id, tar.task_id, tar.requester_id, tar.created_at
)SQL";