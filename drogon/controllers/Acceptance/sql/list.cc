#include "sql.h"

const char* listTaskAcceptanceRequestsSql = R"SQL(
	SELECT id, task_id, requester_id, created_at
	FROM task_acceptance_requests
	 WHERE ($1 IS NULL OR requester_id = $1)
)SQL";