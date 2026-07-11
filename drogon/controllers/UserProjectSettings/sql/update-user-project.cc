#include "update-user-project.h"
const std::string updateUserProjectSql = R"SQL(
    WITH updated_setting AS (
        UPDATE user_project_settings
        SET project_id = $3::uuid,
            excecutor_time_ratio = $4::numeric
        WHERE id = $1::uuid
          AND user_id = $2::uuid
        RETURNING id, user_id, project_id, excecutor_time_ratio
    )
    UPDATE tasks t
    SET excecutor_time_ratio = us.excecutor_time_ratio
    FROM updated_setting us
    WHERE t.project_id = us.project_id
      AND t.completed = false
      AND EXISTS (
          SELECT 1
          FROM executors_tasks et
          WHERE et.task_id = t.id
            AND et.user_id = us.user_id
      );
)SQL";
