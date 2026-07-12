#include "sql.h"

const char* rejectedCountHistorySql = R"SQL(
	SELECT
		tar.requested_at::date AS date,
		COUNT(*) AS rejected
	FROM task_acceptance_records tar
	WHERE tar.accepted = false
	GROUP BY tar.requested_at::date
	ORDER BY date
)SQL";