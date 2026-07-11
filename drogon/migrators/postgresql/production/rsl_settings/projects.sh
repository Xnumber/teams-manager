#!/bin/bash
# 用法: ./settings_db.sh <DB_NAME>
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要設定的資料庫名稱作為參數。"
	exit 1
fi

echo -e "\033[1;36m啟用 orders 資料表 RLS 並建立 tenant_isolation policy...\033[0m"
# 為了多租戶隔離，啟用 orders 資料表的 RLS 並建立 tenant_isolation policy
sudo -u postgres psql -d $DB_NAME <<EOF
ALTER TABLE projects ENABLE ROW LEVEL SECURITY;
CREATE POLICY IF NOT EXISTS tenant_isolation
	ON projects
	USING (
		tenant_id = current_setting('app.tenant_id')::uuid
	);
EOF
