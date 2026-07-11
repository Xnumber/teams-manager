#include "update.h"



const std::string getMilestoneUpdateSql = R"SQL(
	WITH current_milestone AS (
		SELECT id, project_id
		FROM milestones
		WHERE id = $1::uuid
		FOR UPDATE
	),
	updated_milestone AS (
		UPDATE milestones m
		SET
			name = COALESCE(NULLIF($2, ''), m.name),
			description = COALESCE(NULLIF($3, ''), m.description),
			project_id = COALESCE(NULLIF($4, '')::uuid, m.project_id),
			project_name = CASE
				WHEN COALESCE(NULLIF($4, '')::uuid, m.project_id) IS DISTINCT FROM m.project_id THEN
					COALESCE(
						(
							SELECT p.name
							FROM projects p
							WHERE p.id = COALESCE(NULLIF($4, '')::uuid, m.project_id)
							LIMIT 1
						),
						NULLIF($5, ''),
						m.project_name
					)
				ELSE COALESCE(NULLIF($5, ''), m.project_name)
			END,
			team_id = COALESCE(NULLIF($6, '')::uuid, m.team_id),
			team_name = COALESCE(NULLIF($7, ''), m.team_name),
			tenant_id = COALESCE(NULLIF($8, '')::uuid, m.tenant_id),
			tenant_name = COALESCE(NULLIF($9, ''), m.tenant_name),
			start_date = COALESCE(NULLIF($10, '')::date, m.start_date),
			end_date = COALESCE(NULLIF($11, '')::date, m.end_date),
			creator_id = COALESCE(NULLIF($12, '')::uuid, m.creator_id),
			creator_name = COALESCE(NULLIF($13, ''), m.creator_name),
			concurrency_stamp = COALESCE(NULLIF($14, ''), m.concurrency_stamp),
			remark = COALESCE(NULLIF($15, ''), m.remark),
			priority = COALESCE(NULLIF($16, '')::integer, m.priority),
			estimated_remaining_workdays = COALESCE(NULLIF($17, '')::integer, m.estimated_remaining_workdays),
			dependency_milestone_id = COALESCE(NULLIF($18, '')::uuid, m.dependency_milestone_id),
			estimated_start_date = COALESCE(NULLIF($19, '')::date, m.estimated_start_date),
			estimated_end_date = COALESCE(NULLIF($20, '')::date, m.estimated_end_date),
			completed = COALESCE(NULLIF($21, '')::boolean, m.completed),
			is_for_demo = COALESCE(NULLIF($22, '')::boolean, m.is_for_demo)
		WHERE m.id = $1::uuid
		RETURNING m.*
	),
	sync_task_project AS (
		UPDATE tasks t
		SET project_id = um.project_id
		FROM updated_milestone um
		JOIN current_milestone cm ON cm.id = um.id
		WHERE t.milestone_id = um.id
		  AND um.project_id IS DISTINCT FROM cm.project_id
		RETURNING t.id
	)
	SELECT * FROM updated_milestone
)SQL";