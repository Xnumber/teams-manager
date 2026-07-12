#include "sql.h"

const char* acceptedCountHistorySql = R"SQL(
	SELECT
		tar.requested_at::date AS date,
		COUNT(*) AS accepted
	FROM task_acceptance_records tar
	WHERE tar.accepted = true
	GROUP BY tar.requested_at::date
	ORDER BY date
)SQL";