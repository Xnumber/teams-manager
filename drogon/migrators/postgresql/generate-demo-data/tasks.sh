#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要插入資料的資料庫名稱作為參數。"
	exit 1
fi

echo -e "\033[1;36m每個專案隨機產生 100~200 筆 task，租戶隨機指定...\033[0m"

sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH project_list AS (
	SELECT id AS project_id, name AS project_name FROM projects
),
tenant_list AS (
	SELECT id AS tenant_id, name AS tenant_name FROM tenants
),
user_list AS (
	SELECT id AS user_id, username AS user_name FROM users
),
status_list AS (
	SELECT id AS status_id, name AS status_name FROM task_status
),
team_list AS (
	SELECT id AS team_id, name AS team_name FROM teams
),
milestone_list AS (
	SELECT id AS milestone_id, name AS milestone_name, project_id FROM milestones
),
task_data AS (
	SELECT
		gen_random_uuid() AS id,
		t.tenant_id,
		t.tenant_name,
		p.project_id,
		p.project_name,
		u.user_id AS creator_id,
		u.user_name AS creator_name,
		NULL::UUID AS task_type_id,
		'' AS task_type_name,
		'Task_' || p.project_name || '_' || i AS name,
		0 AS excecutor_time_ratio,
		'Demo task for ' || p.project_name || ' 編號' || i AS description,
		gen_random_uuid() AS concurrency_stamp,
		NOW() AS created_at,
		NULL::VARCHAR AS remark,
		CURRENT_DATE + (i || ' days')::interval AS scheduled_start_date,
		CURRENT_DATE + ((i+1) || ' days')::interval AS scheduled_end_date,
		NULL::DATE AS scheduled_completion_date,
		NULL::DATE AS completion_date,
		round(random()::numeric, 2) AS progress,
		false AS completed,
		s.status_id AS status_id,
		s.status_name AS status_name,
		CURRENT_DATE AS status_last_changed,
		'' AS result_description,
		tm.team_id AS team_id,
		tm.team_name AS team_name,
		-- 隨機選一個同 project 的 milestone
		ml.milestone_id,
		ml.milestone_name,
		(floor(random() * 10) + 1)::int AS estimated_working_days,
		(random() > 0.8) AS delayed
	FROM project_list p,
		 LATERAL generate_series(1, (floor(random() * 101) + 100)::int) AS i
		 JOIN LATERAL (
			 SELECT * FROM tenant_list ORDER BY md5(random()::text || i::text) LIMIT 1
		 ) t ON TRUE
		 JOIN LATERAL (
			 SELECT * FROM user_list ORDER BY md5(random()::text || i::text) LIMIT 1
		 ) u ON TRUE
		 JOIN LATERAL (
			 SELECT id AS status_id, name AS status_name
			 FROM task_status
			 ORDER BY md5(random()::text || i::text)
			 LIMIT 1
		 ) s ON TRUE
		 JOIN LATERAL (
			 SELECT * FROM team_list ORDER BY md5(random()::text || i::text) LIMIT 1
		 ) tm ON TRUE
		 LEFT JOIN LATERAL (
			 SELECT milestone_id, milestone_name FROM milestone_list ml WHERE ml.project_id = p.project_id ORDER BY random() LIMIT 1
		 ) ml ON TRUE
)
INSERT INTO tasks (
	id, tenant_id, tenant_name, project_id, project_name, creator_id, creator_name, task_type_id, task_type_name, name,
	excecutor_time_ratio, description, concurrency_stamp, created_at, remark, scheduled_start_date, scheduled_end_date,
	scheduled_completion_date, completion_date, progress, completed, status_id, status_name, status_last_changed,
	result_description, team_id, team_name, milestone_id, milestone_name, estimated_working_days, delayed
)
SELECT id, tenant_id, tenant_name, project_id, project_name, creator_id, creator_name, task_type_id, task_type_name, name,
	excecutor_time_ratio, description, concurrency_stamp, created_at, remark, scheduled_start_date, scheduled_end_date,
	scheduled_completion_date, completion_date, progress, completed, status_id, status_name, status_last_changed,
	result_description, team_id, team_name, milestone_id, milestone_name, estimated_working_days, delayed
FROM task_data
EOF
