const char *getProjectsSql = R"(
    SELECT DISTINCT p.* FROM projects p
    JOIN plans_projects pp ON p.id = pp.project_id
    WHERE pp.plan_id = $1
    ORDER BY p.priority;
)";
const char *getMilestonesSql = R"(
    SELECT DISTINCT m.* FROM milestones m
    JOIN plans_projects pp ON m.project_id = pp.project_id
    WHERE pp.plan_id = $1
    ORDER BY m.priority;
)";
const char *getTasksSql = R"(
    SELECT t.id, 
        t.milestone_id, 
        t.name, 
        t.priority, 
        t.completed, 
        t.progress, 
        t.scheduled_start_date, 
        t.scheduled_end_date,
        t.estimated_workdays,
        t.excecutor_time_ratio,
        t.completion_date
    FROM tasks t
    JOIN plans_projects pp ON t.project_id = pp.project_id
    LEFT JOIN milestones m ON m.id = t.milestone_id
    LEFT JOIN projects p ON p.id = t.project_id
    WHERE pp.plan_id = $1
    ORDER BY p.id, p.priority, m.priority, t.priority;
)";

const char *getDependenciesSql = R"(
    select * from work_item_dependencies
)";

const char *getWorkItemsSql = R"(




WITH project_data AS (
    SELECT jsonb_build_object(
        'id', p.id,
        'tenant_id', p.tenant_id,
        'creator_id', p.creator_id,
        'creator_name', p.creator_name,
        'tenant_name', p.tenant_name,
        'name', p.name,
        'description', p.description,
        'test_count', p.test_count,
        'test_pass_count', p.test_pass_count,
        'test_auto_count', p.test_auto_count,
        'available_developer_count', p.available_developer_count,
        'unit_test_coverage', p.unit_test_coverage,
        'estimated_remaining_workdays', p.estimated_remaining_workdays,
        'priority', p.priority,
        'created_at', p.created_at
    ) AS project
    FROM projects p
    WHERE p.id = ANY($1::uuid[])
    ORDER BY p.priority
),
milestone_data AS (
    SELECT json_agg(jsonb_build_object(
        'id', m.id,
        'name', m.name,
         -- 'description', m.description,
        'project_id', m.project_id,
        'project_name', m.project_name,
        'team_id', m.team_id,
        'team_name', m.team_name,
        'tenant_id', m.tenant_id,
        'tenant_name', m.tenant_name,
        'start_date', m.start_date,
        'end_date', m.end_date,
        -- 'creator_id', m.creator_id,
        --'creator_name', m.creator_name,
        -- 'remark', m.remark,
        'priority', m.priority,
        'estimated_remaining_workdays', m.estimated_remaining_workdays,
        'dependency_milestone_id', m.dependency_milestone_id,
        'estimated_start_date', m.estimated_start_date,
        'estimated_end_date', m.estimated_end_date,
        'completed', m.completed
        -- 'is_for_demo', m.is_for_demo,
        -- 'completion_date', m.completion_date,
        -- 'created_at', m.created_at
    ) ORDER BY m.priority, m.start_date NULLS FIRST, m.created_at, m.id) AS milestones
    FROM milestones m
    WHERE m.project_id = ANY($1::uuid[])
),
task_data AS (
    SELECT json_agg(jsonb_build_object(
        'id', t.id,
        -- 'creator_id', t.creator_id,
        -- 'creator_name', t.creator_name,
        'tenant_id', t.tenant_id,
        'tenant_name', t.tenant_name,
        'project_id', t.project_id,
        'project_name', t.project_name,
        -- 'task_type_id', t.task_type_id,
        -- 'task_type_name', t.task_type_name,
        'name', t.name,
        -- 'description', t.description,
        -- 'remark', t.remark,
        'scheduled_start_date', t.scheduled_start_date,
        'scheduled_end_date', t.scheduled_end_date,
        'scheduled_completion_date', t.scheduled_completion_date,
        'completion_date', t.completion_date,
        'progress', t.progress,
        'completed', t.completed,
        -- 'status_id', t.status_id,
        -- 'status_name', t.status_name,
        -- 'status_last_changed', t.status_last_changed,
        -- 'result_description', t.result_description,
        'team_id', t.team_id,
        'team_name', t.team_name,
        'milestone_id', t.milestone_id,
        'milestone_name', t.milestone_name,
        'delayed', t.delayed,
        'estimated_workdays', t.estimated_workdays,
        'excecutor_time_ratio', t.excecutor_time_ratio,
        'priority', t.priority
        -- 'created_at', t.created_at
    ) ORDER BY t.priority, t.scheduled_start_date, t.created_at, t.id) AS tasks
    FROM tasks t
    WHERE t.project_id = ANY($1::uuid[])
)
SELECT jsonb_build_object(
    'projects', COALESCE((SELECT ARRAY[project] FROM project_data WHERE project IS NOT NULL), ARRAY[]::jsonb[]),
    'milestones', COALESCE((SELECT milestones FROM milestone_data), '[]'::json),
    'tasks', COALESCE((SELECT tasks FROM task_data), '[]'::json)
) AS data
)";
