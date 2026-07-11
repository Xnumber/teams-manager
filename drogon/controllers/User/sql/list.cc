#include "list.h"
const std::string getUserListSql = R"SQL(
    SELECT 
        u.tenant_id,
        u.tenant_name,
        u.team_id,
        u.team_name,
        u.id,
        u.username,
    COUNT(t.id) AS left_tasks,
    COUNT(DISTINCT t.project_id) AS project_count
    FROM USERS u
    LEFT JOIN EXECUTORS_TASKS et
        ON u.id = et.user_id
    LEFT JOIN TASKS t
        ON et.task_id = t.id
        AND t.completed = false
    GROUP BY 
        u.id, 
        u.username;
)SQL";