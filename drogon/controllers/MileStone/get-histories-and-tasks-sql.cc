#include "get-histories-and-tasks-sql.h"
#include <string>

std::string getMilestoneEstimationHistoriesSql()
{
  return R"(
    SELECT
      h.id,
      h.milestone_id,
      h.estimation_date,
      h.complete_date,
      h.optimistic_estimated_complete_date,
      h.pessimistic_estimated_complete_date,
      h.created_at,
      h.estimated_remaining_workdays,
      h.left_tasks
    FROM milestone_estimation_histories h
    WHERE h.milestone_id = $1::uuid
    ORDER BY h.created_at DESC;
  )";
}

std::string getMilestoneTasksSql()
{
  return R"(
    WITH dependency AS (
      SELECT
        m.dependency_milestone_id,
        dm.estimated_end_date AS dependency_estimated_end_date
      FROM milestones m
      LEFT JOIN milestones dm ON dm.id = m.dependency_milestone_id
      WHERE m.id = $1::uuid
    ),
    grouped AS (
      SELECT
        et.user_id AS executor_id,
        et.user_name AS executor_name,
        SUM(t.estimated_workdays / t.excecutor_time_ratio) AS total_estimated_workdays
      FROM tasks t
      LEFT JOIN LATERAL (
        SELECT user_id, user_name
        FROM executors_tasks
        WHERE task_id = t.id
        LIMIT 1
      ) et ON true
      WHERE t.milestone_id = $1::uuid
      AND t.completed = false
      AND et.user_id IS NOT NULL
      GROUP BY et.user_id, et.user_name
    ),
    max_grouped AS (
      SELECT *
      FROM grouped
      WHERE total_estimated_workdays = (SELECT MAX(total_estimated_workdays) FROM grouped)
    )
    SELECT
      mg.executor_id,
      mg.executor_name,
      mg.total_estimated_workdays,
      d.dependency_milestone_id,
      d.dependency_estimated_end_date
    FROM dependency d
    LEFT JOIN max_grouped mg ON true;
  )";
}