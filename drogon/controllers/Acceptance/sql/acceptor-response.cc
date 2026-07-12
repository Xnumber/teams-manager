#include "sql.h"
const char* acceptorResponseAcceptanceRequestSql = R"SQL(
	INSERT INTO task_acceptance_records (
		task_id,
		acceptor_id,
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
		(SELECT created_at FROM task_acceptance_requests WHERE task_id = $1 ORDER BY created_at DESC LIMIT 1),
		$3,
		$4,
		$5,
		$6,
		$7,
		now(),
		now()
	)
	RETURNING id, task_id, acceptor_id, requested_at, accepted, reject_type_id, reject_reason, acceptor_comment, reviewed_at, created_at, updated_at;
)SQL";