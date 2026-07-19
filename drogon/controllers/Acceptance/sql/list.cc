#include "sql.h"

const char* listTaskAcceptanceRequestsSql = R"SQL(
	SELECT id, task_id, requester_id, created_at
	FROM task_acceptance_requests
	WHERE requester_id = $1::uuid
)SQL";