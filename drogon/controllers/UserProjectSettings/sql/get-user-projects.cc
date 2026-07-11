#include "get-user-projects.h"

const std::string getUserProjectsSql = R"SQL(
    SELECT DISTINCT p.id, p.name
    FROM tasks t
        JOIN executors_tasks et ON et.task_id = t.id
        JOIN users u ON u.id = et.user_id
        JOIN projects p ON t.project_id = p.id
        WHERE u.id = $1::uuid
            AND t.completed = false;
)SQL";