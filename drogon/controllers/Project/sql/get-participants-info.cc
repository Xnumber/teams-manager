#include "get-participants-info.h"

const std::string getParticipantsInfoSql = R"SQL(
    SELECT count(distinct u.id) as count, AVG(t.excecutor_time_ratio) AS avg_excecutor_time_ratio, SUM(t.estimated_workdays) AS sum_estimated_workdays
    FROM tasks t
        JOIN executors_tasks et ON et.task_id = t.id
        JOIN users u ON u.id = et.user_id
        JOIN projects p ON t.project_id = p.id
        WHERE p.id = $1::uuid
            AND t.completed = false;
)SQL";