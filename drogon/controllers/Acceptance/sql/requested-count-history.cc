#include "sql.h"

const char* requestedCountHistorySql = R"SQL(
	SELECT
		tar.created_at::date AS date,
		COUNT(*) AS requested
	FROM task_acceptance_requests tar
	GROUP BY tar.created_at::date
	ORDER BY date
)SQL";