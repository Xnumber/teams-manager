#!/bin/bash
# 連線 postgresql 使用 sql 建立 task type 測試資料



DB_NAME="teams_manager"
TASK_TYPE_NAME="你的工作類型名稱"
ANOTHER_TASK_TYPE_NAME="你的另一個工作類型名稱"
TASK_TYPE_DESC="描述內容"
ANOTHER_TASK_TYPE_DESC="Another task type description"

user_name_1="指導一"
user_name_2="指導二"
user_name_3="執行一"
tenant_id=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO tenants (name, description) VALUES ('test tenant', 'test tenant description') RETURNING id;" | head -n 1 | tr -d '[:space:]')

users_id_1=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO users (tenant_id, username, email, password) VALUES ('$tenant_id', '$user_name_1', 'testuser@example.com', 'password') RETURNING id;" | head -n 1 | tr -d '[:space:]')

users_id_2=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO users (tenant_id, username, email, password) VALUES ('$tenant_id', '$user_name_2', 'testuser2@example.com', 'password') RETURNING id;" | head -n 1 | tr -d '[:space:]')

users_id_3=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO users (tenant_id, username, email, password) VALUES ('$tenant_id', '$user_name_3', 'testuser3@example.com', 'password') RETURNING id;" | head -n 1 | tr -d '[:space:]')

project_id=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO projects (tenant_id, name, description) VALUES ('$tenant_id', 'test project', 'test project description') RETURNING id;" | head -n 1 | tr -d '[:space:]')

task_type_id=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO task_types (name, description) VALUES ('$TASK_TYPE_NAME', '$TASK_TYPE_DESC') RETURNING id;" | head -n 1 | tr -d '[:space:]')
another_task_type_id=$(sudo -u postgres psql -d "$DB_NAME" -t -A -c \
"INSERT INTO task_types (name, description) VALUES ('$ANOTHER_TASK_TYPE_NAME', '$ANOTHER_TASK_TYPE_DESC') RETURNING id;" | head -n 1 | tr -d '[:space:]')



result_json=$(jq -n \
--arg status "success" \
--arg tenant_id "$tenant_id" \
--arg project_id "$project_id" \
--arg task_type_id "$task_type_id" \
--arg another_task_type_id "$another_task_type_id" \
--arg another_task_type_name "$ANOTHER_TASK_TYPE_NAME" \
--arg users_id_1 "$users_id_1" \
--arg users_id_2 "$users_id_2" \
--arg users_id_3 "$users_id_3" \
--arg user_name_1 "$user_name_1" \
--arg user_name_2 "$user_name_2" \
--arg user_name_3 "$user_name_3" \
'{tenant_id: $tenant_id, project_id: $project_id, task_type_id: $task_type_id, another_task_type_id: $another_task_type_id, another_task_type_name: $another_task_type_name, users_id_1: $users_id_1, users_id_2: $users_id_2, users_id_3: $users_id_3, status: $status, user_name_1: $user_name_1, user_name_2: $user_name_2, user_name_3: $user_name_3}')
echo "$result_json"