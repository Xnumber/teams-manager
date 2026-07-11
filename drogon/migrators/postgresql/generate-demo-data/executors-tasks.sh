#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要插入資料的資料庫名稱作為參數。"
	exit 1
fi

echo -e "\033[1;36m每個 task 隨機配對一個 user...\033[0m"

sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH task_list AS (
	SELECT id AS task_id FROM tasks
),
user_list AS (
	SELECT id AS user_id, username AS user_name FROM users
),
exec_data AS (
	SELECT
		t.task_id,
		u.user_id,
		u.user_name
	FROM task_list t
        JOIN LATERAL (
            SELECT id AS user_id, username AS user_name
            FROM users
			ORDER BY md5(t.task_id::text || random()::text)
            LIMIT 1
        ) u ON TRUE
)
INSERT INTO executors_tasks (task_id, user_id, user_name)
SELECT task_id, user_id, user_name FROM exec_data
ON CONFLICT DO NOTHING;
EOF