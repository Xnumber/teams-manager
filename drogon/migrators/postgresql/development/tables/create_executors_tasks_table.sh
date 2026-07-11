#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
    echo "請提供要建立資料表的資料庫名稱作為參數。"
    exit 1
fi
echo -e "\033[1;36m建立工作執行者資料表...\033[0m"



sudo -u postgres psql -d $DB_NAME <<EOF
CREATE TABLE IF NOT EXISTS executors_tasks (
    task_id UUID REFERENCES tasks(id) ON DELETE CASCADE,
    user_id UUID REFERENCES users(id) ON DELETE CASCADE,
    user_name VARCHAR(100) NOT NULL,
    PRIMARY KEY (task_id, user_id)
);
EOF