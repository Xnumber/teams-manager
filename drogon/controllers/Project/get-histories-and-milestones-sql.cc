#include "get-histories-and-milestones-sql.h"
#include <string>




// -- h.id,
// -- h.project_id,
// -- h.created_at,


std::string getProjectEstimationHistoriesSql()
{
    return R"(
      WITH latest_milestone_history AS (
        SELECT
          m.name AS milestone_name,
          meh.complete_date
        FROM milestone_estimation_histories meh
        LEFT JOIN milestones m ON m.id = meh.milestone_id
        WHERE meh.project_id = $1::uuid
        AND m.is_for_demo = true
        AND m.completed = false
      ),
      latest_complete_date_history AS (
        SELECT milestone_name, complete_date
        FROM latest_milestone_history
        ORDER BY complete_date DESC NULLS LAST
        LIMIT 1
      ),
      earliest_complete_date_history AS (
        SELECT milestone_name, complete_date
        FROM latest_milestone_history
        ORDER BY complete_date ASC NULLS LAST
        LIMIT 1
      )
      SELECT
        h.estimation_date,
        h.complete_date,
        h.optimistic_estimated_complete_date,
        h.pessimistic_estimated_complete_date,
        h.estimated_remaining_workdays,
        h.left_milestones,
        lch.milestone_name AS latest_milestone_name,
        lch.complete_date AS latest_milestone_complete_date,
        ech.milestone_name AS earliest_milestone_name,
        ech.complete_date AS earliest_milestone_complete_date
      FROM project_estimation_histories h
      LEFT JOIN latest_complete_date_history lch ON true
      LEFT JOIN earliest_complete_date_history ech ON true
      WHERE h.project_id = $1::uuid
      ORDER BY h.created_at DESC;
    )";
}
std::string getProjectTasksSql()
{
  return R"(
    WITH grouped AS (
      SELECT
        et.user_id AS executor_id,
        et.user_name AS executor_name,
        SUM(t.estimated_workdays) AS total_estimated_workdays
      FROM tasks t
      LEFT JOIN LATERAL (
        SELECT user_id, user_name
        FROM executors_tasks
        WHERE task_id = t.id
        LIMIT 1
      ) et ON true
      WHERE t.project_id = $1::uuid
      AND t.completed = false
      AND et.user_id IS NOT NULL
      GROUP BY et.user_id, et.user_name
    )
    SELECT *
    FROM grouped
    WHERE total_estimated_workdays = (SELECT MAX(total_estimated_workdays) FROM grouped);
  )";
}