#!/bin/bash
DB_NAME="teams_manager"
APP_USER="clement"
APP_PASS="teams_manager"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# 先執行 drop_db.sh
bash "$SCRIPT_DIR/drop_db.sh" "$DB_NAME"

echo -e "\033[1;36m建立資料庫中...\033[0m"

sudo -u postgres psql <<EOF

-- 建立資料庫
CREATE DATABASE $DB_NAME;

-- 建立使用者
CREATE USER $APP_USER WITH PASSWORD '$APP_PASS';

-- 給權限
GRANT ALL PRIVILEGES ON DATABASE $DB_NAME TO $APP_USER;

\c $DB_NAME



GRANT USAGE, CREATE ON SCHEMA public TO $APP_USER;
EOF
echo -e "\033[1;34m Running Migration...\033[0m"
cd "$SCRIPT_DIR/../../../liquibase"
# /home/clement/projects/project-teams-manager/drogon/liquibase
# /home/clement/projects/project-teams-manager/drogon/migrators/postgresql/development/setup_db.sh
liquibase update
# echo -e "\033[1;34m處理資料表...\033[0m"
# bash "$SCRIPT_DIR/tables/create_tenants_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_teams_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_users_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_projects_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_milestones_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_task_types_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_task_status_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_tasks_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_mentors_tasks_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/tables/create_executors_tasks_table.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/rls_settings/projects.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/rls_settings/task_types.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/rls_settings/tasks.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/rls_settings/teams.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/rls_settings/tenants.sh" "$DB_NAME"
# bash "$SCRIPT_DIR/rls_settings/users.sh" "$DB_NAME"
echo -e "\033[1;32m建立資料庫與資料表完成！\033[0m"