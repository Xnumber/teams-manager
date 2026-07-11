#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要插入資料的資料庫名稱作為參數。"
    exit 1
fi
echo -e "\033[1;36m每個專案隨機產生 1~10 筆 milestone...\033[0m"

sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH project_list AS (
    SELECT id AS project_id, name AS project_name, creator_id, creator_name FROM projects
),
creator_list AS (
    SELECT id, username FROM users
),
milestone_data AS (
    SELECT
        gen_random_uuid() AS id,
        p.project_id,
        p.project_name,
        'Milestone_' || p.project_name || '_' || i AS name,
        p.project_name || ' 的工作大項 ' || i AS description,
        (CURRENT_DATE + ((i-1)*10 || ' days')::interval)::date AS start_date,
        (CURRENT_DATE + ((i*10) || ' days')::interval)::date AS end_date,
        NOW() AS created_at,
        p.creator_id AS creator_id,
        p.creator_name AS creator_name,
        gen_random_uuid() AS concurrency_stamp,
        NULL::VARCHAR AS remark
    FROM project_list p,
         LATERAL generate_series(1, (floor(random() * 10) + 1)::int) AS i
)
INSERT INTO milestones (
    id, name, description, project_id, project_name, start_date, end_date, created_at, creator_id, creator_name, concurrency_stamp, remark
)
SELECT id, name, description, project_id, project_name, start_date, end_date, created_at, creator_id, creator_name, concurrency_stamp, remark
FROM milestone_data;
EOF