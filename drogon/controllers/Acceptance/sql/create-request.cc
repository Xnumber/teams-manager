#include "sql.h"

const char* createTaskAcceptanceRequestSql = R"SQL(
WITH ins AS (
	INSERT INTO task_acceptance_requests (requester_id, task_id)
	VALUES ($1, $2)
	RETURNING id, task_id, requester_id, created_at
), acc AS (
	INSERT INTO task_acceptor (task_id, user_id)
	SELECT ins.task_id, (jsonb_array_elements_text($3::jsonb))::uuid
	FROM ins
	ON CONFLICT DO NOTHING
	RETURNING task_id, user_id
)
SELECT
	ins.id,
	ins.task_id,
	ins.requester_id,
	ins.created_at,
	COALESCE(jsonb_agg(to_jsonb(acc.*)) FILTER (WHERE acc.user_id IS NOT NULL), '[]'::jsonb) AS acceptors
FROM ins
LEFT JOIN acc ON acc.task_id = ins.task_id
GROUP BY ins.id, ins.task_id, ins.requester_id, ins.created_at;
)SQL";