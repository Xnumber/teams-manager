#include "get-overview.h"

const std::string getProjectOverviewSql = R"SQL(
    SELECT
        p.id AS project_id,
        COALESCE(m.remaining_milestones_count, 0) AS remaining_milestones_count,
        COALESCE(pt.participating_teams_count, 0) AS participating_teams_count,
        COALESCE(e.executors_count, 0) AS executors_count,
        (
            SELECT peh.complete_date
            FROM project_estimation_histories peh
            WHERE peh.project_id = p.id
            ORDER BY peh.estimation_date DESC, peh.complete_date DESC
            LIMIT 1
        ) AS latest_complete_date,
        (
            SELECT m2.name
            FROM milestone_estimation_histories meh
            INNER JOIN milestones m2 ON m2.id = meh.milestone_id
            WHERE meh.project_id = p.id
              AND m2.completed = false
              AND m2.is_for_demo = true
            ORDER BY meh.complete_date ASC, meh.created_at ASC
            LIMIT 1
        ) AS earliest_incomplete_demo_milestone_name,
        (
                SELECT meh.complete_date
                FROM milestone_estimation_histories meh
                INNER JOIN milestones m2 ON m2.id = meh.milestone_id
                WHERE meh.project_id = p.id
                    AND m2.completed = false
                    AND m2.is_for_demo = true
                ORDER BY meh.complete_date ASC, meh.created_at ASC
                LIMIT 1
        ) AS earliest_incomplete_demo_milestone_complete_date
    FROM projects p
    
    LEFT JOIN (
        SELECT
            project_id,
            COUNT(*)::integer AS remaining_milestones_count
        FROM milestones
        WHERE completed = false
        GROUP BY project_id
    ) m ON m.project_id = p.id
    LEFT JOIN (
        SELECT
            t.project_id,
            COUNT(DISTINCT t.team_id)::integer AS participating_teams_count
        FROM (
            SELECT project_id, team_id
            FROM milestones
            WHERE team_id IS NOT NULL
            UNION
            SELECT project_id, team_id
            FROM tasks
            WHERE team_id IS NOT NULL
        ) t
        GROUP BY t.project_id
    ) pt ON pt.project_id = p.id
    LEFT JOIN (
        SELECT
            t.project_id,
            COUNT(DISTINCT et.user_id)::integer AS executors_count
        FROM tasks t
        INNER JOIN executors_tasks et ON et.task_id = t.id
        GROUP BY t.project_id
    ) e ON e.project_id = p.id
    WHERE p.id = $1::uuid
)SQL";
