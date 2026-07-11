#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要插入資料的資料庫名稱作為參數。"
	exit 1
fi

echo -e "\033[1;36m每個 team 隨機產生 1~10 筆 task type...\033[0m"

sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH team_list AS (
	SELECT id AS team_id, name AS team_name FROM teams
),
task_type_data AS (
	SELECT
		gen_random_uuid() AS id,
		'TaskType_' || t.team_name || '_' || i AS name,
		t.team_id,
		t.team_name,
		t.team_name || ' 的工作類型 ' || i AS description,
		gen_random_uuid() AS concurrency_stamp,
		NOW() AS created_at
	FROM team_list t,
		 LATERAL generate_series(1, (floor(random() * 10) + 1)::int) AS i
)
INSERT INTO task_types (id, name, team_id, team_name, description, concurrency_stamp, created_at)
SELECT id, name, team_id, team_name, description, concurrency_stamp, created_at FROM task_type_data
ON CONFLICT (name) DO NOTHING;
EOF
