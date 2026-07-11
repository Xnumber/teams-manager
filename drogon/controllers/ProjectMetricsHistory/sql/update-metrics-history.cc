#include "sql.h"
/**
 * 將每個project_id取出，更新每一個project的metrics_history， 更新為當前的未完成工作數量。
 */
const char* updateProjectMetricsHistorySql = R"(
    INSERT INTO project_metrics_history (
        id,
        project_id,
        date,
        task_count,
        task_added_count,
        task_completed_count,
        milestone_count,
        milestone_added_count,
        milestone_completed_count,
        changed_at
    )
    WITH task_stats AS (
        SELECT
            project_id,
            COUNT(*) FILTER (
                WHERE completed = false
            ) AS task_count,
            COUNT(*) FILTER (
                WHERE created_at >= CURRENT_DATE
                AND created_at < CURRENT_DATE + INTERVAL '1 day'
            ) AS task_added_count,
            COUNT(*) FILTER (
                WHERE completion_date >= CURRENT_DATE
                AND completion_date < CURRENT_DATE + INTERVAL '1 day'
                AND completed = true
            ) AS task_completed_count
        FROM tasks 
        WHERE project_id IS NOT NULL
        GROUP BY project_id
    ),
    milestone_stats AS (
        SELECT
            project_id,
            COUNT(*) FILTER (
                WHERE completed = false
            ) AS milestone_count,
            COUNT(*) FILTER (
                WHERE created_at >= CURRENT_DATE
                AND created_at < CURRENT_DATE + INTERVAL '1 day'
            ) AS milestone_added_count,
            COUNT(*) FILTER (
                WHERE completion_date >= CURRENT_DATE
                AND completion_date < CURRENT_DATE + INTERVAL '1 day'
                AND completed = true
            ) AS milestone_completed_count
        FROM milestones
        GROUP BY project_id
    )
    SELECT
        gen_random_uuid(),
        COALESCE(
            t.project_id,
            m.project_id
        ) AS project_id,
        CURRENT_DATE,
        COALESCE(t.task_count, 0),
        COALESCE(t.task_added_count, 0),
        COALESCE(t.task_completed_count, 0),
        COALESCE(m.milestone_count, 0),
        COALESCE(m.milestone_added_count, 0),
        COALESCE(m.milestone_completed_count, 0),
        NOW()
    FROM task_stats t
    FULL OUTER JOIN milestone_stats m
        ON t.project_id = m.project_id
    ON CONFLICT (project_id, date)
    DO UPDATE SET
        task_count = EXCLUDED.task_count,
        task_added_count = EXCLUDED.task_added_count,
        task_completed_count = EXCLUDED.task_completed_count,
        milestone_count = EXCLUDED.milestone_count,
        milestone_added_count = EXCLUDED.milestone_added_count,
        milestone_completed_count = EXCLUDED.milestone_completed_count,
        changed_at = NOW();
)";


// 每日新增
// SELECT
//     project_id,
//     project_name,
//     DATE(created_at) AS date,
//     COUNT(*) AS task_count
// FROM public.tasks
// WHERE completed = false
// GROUP BY
//     project_id,
//     project_name,
//     DATE(created_at)
// ORDER BY
//     DATE(created_at),
//     project_name;


// 每日減少
// SELECT
//     project_id,
//     project_name,
//     DATE(completion_date) AS date,
//     COUNT(*) AS task_count
// FROM public.tasks
// WHERE completed = false
// GROUP BY
//     project_id,
//     project_name,
//     DATE(completion_date)
// ORDER BY
//     DATE(completion_date),
//     project_name;