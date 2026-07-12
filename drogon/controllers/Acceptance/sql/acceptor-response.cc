#include "sql.h"
const char* acceptorResponseAcceptanceRequestSql = R"SQL(
	WITH up AS (
		UPDATE task_acceptor
		SET accepted = $4
		WHERE task_id = $1
		  AND user_id = $2
		  AND request_id = $3
	)
	INSERT INTO task_acceptance_records (
		task_id,
		acceptor_id,
		request_id,
		requested_at,
		accepted,
		reject_type_id,
		reject_reason,
		acceptor_comment,
		reviewed_at,
		created_at,
		updated_at
	)
	VALUES (
		$1,
		$2,
		$3,
		(SELECT created_at FROM task_acceptance_requests WHERE id = $3),
		$4,
		$5,
		$6,
		$7,
		$8,
		now(),
		now()
	)
	RETURNING id, task_id, acceptor_id, request_id, requested_at, accepted, reject_type_id, reject_reason, acceptor_comment, reviewed_at, created_at, updated_at;
)SQL";