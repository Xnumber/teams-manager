
#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要插入資料的資料庫名稱作為參數。"
	exit 1
fi

echo -e "\033[1;36m每個租戶隨機產生 2~5 筆團隊...\033[0m"

sudo -u postgres psql -d "$DB_NAME" <<EOF
WITH tenant_list AS (
	SELECT id, name, (floor(random() * 10))::int AS team_count FROM tenants
),
team_data AS (
	SELECT
		t.id AS tenant_id,
		t.name AS tenant_name,
		'團隊 ' || t.name || '_' || i AS name,
		t.name || ' 團隊' || i AS description
	FROM tenant_list t,
			 generate_series(1, t.team_count) AS i
)
INSERT INTO teams (tenant_id, tenant_name, name, description)
SELECT tenant_id, tenant_name, name, description FROM team_data
ON CONFLICT (name) DO NOTHING;
EOF
