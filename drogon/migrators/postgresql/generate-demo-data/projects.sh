#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
		echo "請提供要插入資料的資料庫名稱作為參數。"
		exit 1
fi

echo -e "\033[1;36m每個租戶隨機產生 1~10 筆 project...\033[0m"



sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH tenant_list AS (
	SELECT id AS tenant_id, name AS tenant_name FROM tenants
),
creator_list AS (
	SELECT id, username, tenant_id FROM users
),
project_data AS (
	SELECT
		gen_random_uuid() AS id,
		t.tenant_id,
		c.id AS creator_id,
		c.username AS creator_name,
		t.tenant_name,
		'Project_' || t.tenant_name || '_' || i AS name,
		t.tenant_name || ' 的專案 ' || i AS description,
		(floor(random() * 100)::int) AS test_count,
		(floor(random() * 100)::int) AS test_pass_count,
		(floor(random() * 100)::int) AS test_auto_count,
		(floor(random() * 20)::int) AS available_developer_count,
		round((random() * 100)::numeric, 2) AS unit_test_coverage,
		gen_random_uuid() AS concurrency_stamp,
		NOW() AS created_at
	FROM tenant_list t,
			 LATERAL generate_series(1, (floor(random() * 10) + 1)::int) AS i
			 JOIN LATERAL (
				 SELECT * FROM creator_list c WHERE c.tenant_id = t.tenant_id ORDER BY random() LIMIT 1
			 ) c ON TRUE
)
INSERT INTO projects (
	id, tenant_id, creator_id, creator_name, tenant_name, name, description,
	test_count, test_pass_count, test_auto_count, available_developer_count, unit_test_coverage, concurrency_stamp, created_at
)
SELECT id, tenant_id, creator_id, creator_name, tenant_name, name, description,
			 test_count, test_pass_count, test_auto_count, available_developer_count, unit_test_coverage, concurrency_stamp, created_at
FROM project_data
ON CONFLICT (name) DO NOTHING;
EOF
