#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要建立資料表的資料庫名稱作為參數。"
    exit 1
fi
echo -e "\033[1;36m建立專案資料表...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF

CREATE TABLE IF NOT EXISTS "projects" (
    -- id SERIAL PRIMARY KEY,
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    tenant_id UUID NOT NULL,
    creator_id UUID NOT NULL,
    creator_name VARCHAR(100) NOT NULL,
    tenant_name VARCHAR(100) NOT NULL,
    name VARCHAR(100) UNIQUE NOT NULL,
    description TEXT,
    test_count INTEGER DEFAULT 0,
    test_pass_count INTEGER DEFAULT 0,
    test_auto_count INTEGER DEFAULT 0,
    available_developer_count INTEGER DEFAULT 0,
    unit_test_coverage DECIMAL(5, 2) DEFAULT 0.00,
    concurrency_stamp VARCHAR(36) NOT NULL DEFAULT gen_random_uuid(),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT fk_tenant FOREIGN KEY (tenant_id) REFERENCES tenants(id) ON DELETE NO ACTION
);
EOF