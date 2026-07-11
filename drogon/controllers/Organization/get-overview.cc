#include "get-overview.h"

const std::string getOrganizationOverviewSql = R"SQL(
    WITH teams_count AS (
        SELECT COUNT(*) AS teams_count
        FROM teams
    ),
    projects_count AS (
        SELECT COUNT(*) AS projects_count
        FROM projects
    ),
    users_count AS (
        SELECT COUNT(*) AS users_count
        FROM users
    ),
    members_count AS (
        SELECT COUNT(*) AS members_count
        FROM users
    ),
    tasks_list AS (
        SELECT *
        FROM tasks
        WHERE completed = false
    ),
    tasks_count AS (
        SELECT COUNT(*) AS tasks_count
        FROM tasks_list
    ),
    tasks_estimated_workdays AS (
        SELECT SUM(estimated_workdays) AS tasks_estimated_workdays
        FROM tasks_list
    ),
    milestones_count AS (
        SELECT COUNT(*) AS milestones_count
        FROM milestones
    )
    SELECT 
        (SELECT teams_count FROM teams_count) AS teams_count,
        (SELECT projects_count FROM projects_count) AS projects_count,
        (SELECT users_count FROM users_count) AS users_count,
        (SELECT members_count FROM members_count) AS members_count,
        (SELECT tasks_count FROM tasks_count) AS tasks_count,
        (SELECT tasks_estimated_workdays FROM tasks_estimated_workdays) AS tasks_estimated_workdays,
        (SELECT milestones_count FROM milestones_count) AS milestones_count
)SQL";