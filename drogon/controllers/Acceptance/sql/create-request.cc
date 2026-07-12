#include "sql.h"

const char* createTaskAcceptanceRequestSql = R"SQL(
	INSERT INTO task_acceptance_requests (requester_id, task_id)
	VALUES ($1, $2)
	RETURNING id, task_id, requester_id, created_at;
)SQL";