#include "list.h"

const std::string getProjectsListSql = R"SQL(
	SELECT
		pr.name,
		pr.priority,
		pr.description,
		COUNT(DISTINCT ms.id) FILTER (
			WHERE COALESCE(ms.completed, false) = false
		)::INT AS milestones_count_in_queue,
		COUNT(DISTINCT tk.id) FILTER (
			WHERE tk.status_name = '排隊中'
			OR LOWER(tk.status_name) = 'queued'
		)::INT AS tasks_count_in_queue,
		pr.id,
		pr.concurrency_stamp,
		pr.created_at,
		pr.tenant_id
	FROM projects pr
	LEFT JOIN milestones ms
		ON ms.project_id = pr.id
	LEFT JOIN tasks tk
		ON tk.project_id = pr.id
		AND tk.completed = false
	WHERE (
		NULLIF($1, '') IS NULL
		OR pr.name ILIKE '%' || $1 || '%'
		OR COALESCE(pr.description, '') ILIKE '%' || $1 || '%'
	)
	GROUP BY
		pr.name,
		pr.priority,
		pr.description,
		pr.id,
		pr.concurrency_stamp,
		pr.created_at,
		pr.tenant_id
	ORDER BY pr.priority ASC, pr.created_at DESC
)SQL";