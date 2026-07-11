#pragma once
const char* getTaskStatisticsSql = R"(
SELECT 
    u.username,
    COUNT(DISTINCT t.id) AS tasks_count,
    COUNT(DISTINCT t.project_id) FILTER (WHERE t.project_id IS NOT NULL) AS participated_projects_count
FROM users u
LEFT JOIN (
    -- 通過 executors_tasks 表關聯
    SELECT et.user_id, t.id, t.project_id
    FROM executors_tasks et
    INNER JOIN tasks t ON et.task_id = t.id
    WHERE t.completed = true
      AND t.team_id = $1
      AND DATE_TRUNC('month', t.created_at) = DATE_TRUNC('month', $2::timestamp)
    
    UNION ALL
    
    -- 通過 mentors_tasks 表關聯
    SELECT mt.user_id, t.id, t.project_id
    FROM mentors_tasks mt
    INNER JOIN tasks t ON mt.task_id = t.id
    WHERE t.completed = true
      AND t.team_id = $1
      AND DATE_TRUNC('month', t.created_at) = DATE_TRUNC('month', $2::timestamp)
) t ON u.id = t.user_id
WHERE u.team_id = $1
GROUP BY u.id, u.username
ORDER BY u.username
)";
// 寫一段sql , for postgresql, c++ drogon 中使用
// 目標:
// 統計user 完成的 tasks數量、完成的tasks所參與project的數量
// 條件:
// 可指定user的 team_id, task的created_at的月份
// 輸出欄位
// username, tasks_count, participated_projects_count