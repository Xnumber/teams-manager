#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要建立資料表的資料庫名稱作為參數。"
    exit 1
fi

# 建立工作大項資料表

# milestone: 一個工作可屬於一個工作大項，可為 null，刪除工作大項時不連帶刪除工作
# 新增 team、project、tenant 關聯欄位

echo -e "\033[1;36m建立工作大項資料表...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF


CREATE TABLE IF NOT EXISTS "milestones" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(100) NOT NULL,
    description TEXT,
    project_id UUID,
    project_name VARCHAR(100),
    team_id UUID,
    team_name VARCHAR(100),
    tenant_id UUID,
    tenant_name VARCHAR(100),
    start_date DATE,
    end_date DATE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    creator_id UUID,
    creator_name VARCHAR(100),
    concurrency_stamp VARCHAR(36) NOT NULL DEFAULT gen_random_uuid(),
    remark VARCHAR(2000),
    CONSTRAINT fk_project FOREIGN KEY(project_id) REFERENCES projects(id) ON DELETE SET NULL,
    CONSTRAINT fk_team FOREIGN KEY(team_id) REFERENCES teams(id) ON DELETE SET NULL,
    CONSTRAINT fk_tenant FOREIGN KEY(tenant_id) REFERENCES tenants(id) ON DELETE SET NULL
);

EOF
