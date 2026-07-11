#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要建立資料表的資料庫名稱作為參數。"
    exit 1
fi

echo -e "\033[1;36m建立使用者建立資料表...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF

CREATE TABLE IF NOT EXISTS "users" (
    -- id SERIAL PRIMARY KEY,
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    tenant_id UUID NOT NULL,
    tenant_name VARCHAR(50) NOT NULL,
    team_id UUID NOT NULL,
    team_name VARCHAR(50) NOT NULL,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    concurrency_stamp VARCHAR(36) NOT NULL DEFAULT gen_random_uuid(),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    -- 外鍵：每個 user 屬於一個 tenant，刪除 tenant 時連動刪除 user
    CONSTRAINT fk_tenant FOREIGN KEY (tenant_id) REFERENCES tenants(id) ON DELETE CASCADE
);
EOF