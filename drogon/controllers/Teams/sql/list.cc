#include "list.h"

const std::string getTeamsListSql = R"SQL(
SELECT
	tm.concurrency_stamp,
	tm.description,
	COUNT(tk.id) FILTER (
		WHERE tk.status_name = '排隊中'
		   OR LOWER(tk.status_name) = 'queued'
	)::INT AS tasks_count_in_queue,
	COUNT(tk.id) FILTER (
		WHERE COALESCE(tk.completed, false) = false
	)::INT AS left_task,
	tm.id,
	tm.name,
	tm.tenant_id,
	COALESCE(tt.name, tm.tenant_name) AS tenant_name
FROM teams tm
LEFT JOIN tenants tt
	ON tt.id = tm.tenant_id
LEFT JOIN tasks tk
	ON tk.team_id = tm.id
GROUP BY
	tm.concurrency_stamp,
	tm.description,
	tm.id,
	tm.name,
	tm.tenant_id,
	tt.name,
	tm.tenant_name
ORDER BY tm.created_at DESC
)SQL";