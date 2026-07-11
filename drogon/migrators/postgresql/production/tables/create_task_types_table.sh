
#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要建立資料表的資料庫名稱作為參數。"
	exit 1
fi
echo -e "\033[1;36m建立工作類型資料表...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF

CREATE TABLE IF NOT EXISTS "task_types" (
	id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
	name VARCHAR(100) UNIQUE NOT NULL,
	team_id UUID,
	team_name VARCHAR(100),
	description TEXT,
	concurrency_stamp VARCHAR(36) NOT NULL DEFAULT gen_random_uuid(),
	created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
	CONSTRAINT fk_team
		FOREIGN KEY(team_id)
		REFERENCES teams(id)
		ON DELETE SET NULL
);
EOF
