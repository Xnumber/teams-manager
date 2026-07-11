#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要建立資料表的資料庫名稱作為參數。"
    exit 1
fi

echo -e "\033[1;36m建立工作狀態管理資料表...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF

CREATE TABLE IF NOT EXISTS "task_status" (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(20) NOT NULL UNIQUE,
    description VARCHAR(100)
);

EOF

echo -e "\033[1;36m插入預設工作狀態...\033[0m"
sudo -u postgres psql -d $DB_NAME <<EOF
INSERT INTO "task_status" (name, description) VALUES
    ('排隊中', '工作已排隊等待處理'),
    ('進行中', '工作正在進行中'),
    ('完成', '工作已完成'),
    ('封存', '工作已封存')
ON CONFLICT (name) DO NOTHING;
EOF
