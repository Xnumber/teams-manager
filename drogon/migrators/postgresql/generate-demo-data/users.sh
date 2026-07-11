#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
		echo "請提供要插入資料的資料庫名稱作為參數。"
		exit 1
fi

echo -e "\033[1;36m隨機產生 100 個 user...\033[0m"

sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH team_list AS (
	SELECT t.id AS team_id, t.name AS team_name, t.tenant_id, t.tenant_name FROM teams t
),
user_data AS (
	SELECT
		gen_random_uuid() AS id,
		t.tenant_id,
		t.tenant_name,
		t.team_id,
		t.team_name,
		'user_' || t.team_id || '_' || i AS username,
		'user_' || t.team_id || '_' || i || '@demo.com' AS email,
		'password' AS password,
		gen_random_uuid() AS concurrency_stamp,
		NOW() AS created_at
	FROM team_list t,
		 LATERAL generate_series(1, (floor(random() * 100) + 1)::int) AS i
)
INSERT INTO users (id, tenant_id, tenant_name, team_id, team_name, username, email, password, concurrency_stamp, created_at)
SELECT id, tenant_id, tenant_name, team_id, team_name, username, email, password, concurrency_stamp, created_at FROM user_data
ON CONFLICT (username) DO NOTHING;
EOF
