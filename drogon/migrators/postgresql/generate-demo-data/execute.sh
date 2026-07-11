#!/bin/bash
DB_NAME="$1"
if [ -z "$DB_NAME" ]; then
	echo "請提供要操作的資料庫名稱作為參數。"
	exit 1
fi

# 執行 tenants.sh 產生 demo 資料
DIR="$(cd "$(dirname "$0")" && pwd)"
"$DIR/tenants.sh" "$DB_NAME"
"$DIR/teams/teams.sh" "$DB_NAME"
"$DIR/task-type.sh" "$DB_NAME"
"$DIR/task-status.sh" "$DB_NAME"
"$DIR/users.sh" "$DB_NAME"
"$DIR/projects.sh" "$DB_NAME"
"$DIR/milestones.sh" "$DB_NAME"
"$DIR/tasks.sh" "$DB_NAME"
"$DIR/executors-tasks.sh" "$DB_NAME"