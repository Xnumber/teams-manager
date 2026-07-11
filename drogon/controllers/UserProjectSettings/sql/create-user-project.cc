#include "create-user-project.h"

const std::string createUserProjectSql = R"SQL(
		WITH upsert_setting AS (
				INSERT INTO user_project_settings (user_id, project_id, excecutor_time_ratio)
				VALUES ($1::uuid, $2::uuid, $3::numeric)
				ON CONFLICT (user_id, project_id)
				DO UPDATE SET excecutor_time_ratio = EXCLUDED.excecutor_time_ratio
				RETURNING user_id, project_id, excecutor_time_ratio
		)
		UPDATE tasks t
		SET excecutor_time_ratio = us.excecutor_time_ratio
		FROM upsert_setting us
		WHERE t.project_id = us.project_id
			AND t.completed = false
			AND EXISTS (
					SELECT 1
					FROM executors_tasks et
					WHERE et.task_id = t.id
						AND et.user_id = us.user_id
			);
)SQL";