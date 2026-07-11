#!/bin/bash
# 執行 tasks CRUD 測試流程
# 參數: project_id, task_type_id, name, description

project_id_send="${1:-}"
name_send="test task"
description_send="test task description"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 準備測試資料
test_data=$(bash "$SCRIPT_DIR/prepare_data.sh")
project_id=$(echo "$test_data" | jq -r '.project_id')
task_type_id=$(echo "$test_data" | jq -r '.task_type_id')
mentors_id_1=$(echo "$test_data" | jq -r '.users_id_1')
mentors_id2=$(echo "$test_data" | jq -r '.users_id_2')
executors_id1=$(echo "$test_data" | jq -r '.users_id_3')
user_name_1=$(echo "$test_data" | jq -r '.user_name_1')
user_name_2=$(echo "$test_data" | jq -r '.user_name_2')
user_name_3=$(echo "$test_data" | jq -r '.user_name_3')
another_task_type_id=$(echo "$test_data" | jq -r '.another_task_type_id')
another_task_type_name=$(echo "$test_data" | jq -r '.another_task_type_name')
# 執行 create.sh 來建立工作
createTaskResult=$(bash "$SCRIPT_DIR/create.sh" "$project_id" "$task_type_id" "$mentors_id_1" "$mentors_id2" "$executors_id1" "$user_name_1" "$user_name_2" "$user_name_3")
createStatus=$(echo "$createTaskResult" | jq -r '.status')
task_id=$(echo "$createTaskResult" | jq -r '.id')
another_task_id=$(echo "$createTaskResult" | jq -r '.another_task_id')
# 執行 get.sh 來查詢工作，依照 get.sh 參數順序傳入 createTaskResult 的欄位
getTaskResult=$(bash "$SCRIPT_DIR/get.sh" \
    "$task_id" \
    "$(echo "$createTaskResult" | jq -r '.project_id')" \
    "$(echo "$createTaskResult" | jq -r '.task_type_id')" \
    "$(echo "$createTaskResult" | jq -r '.name')" \
    "$(echo "$createTaskResult" | jq -r '.description')" \
    "$(echo "$createTaskResult" | jq -r '.creator_id')" \
    "$(echo "$createTaskResult" | jq -r '.creator_name')" \
    "$(echo "$createTaskResult" | jq -r '.project_name')" \
    "$(echo "$createTaskResult" | jq -r '.task_type_name')" \
    "$(echo "$createTaskResult" | jq -r '.remark')" \
    "$(echo "$createTaskResult" | jq -r '.scheduled_start_date')" \
    "$(echo "$createTaskResult" | jq -r '.scheduled_end_date')" \
    "$(echo "$createTaskResult" | jq -r '.completion_date')" \
    "$(echo "$createTaskResult" | jq -r '.progress')" \
    "$(echo "$createTaskResult" | jq -r '.completed')" \
    "$(echo "$createTaskResult" | jq -r '.status_val')" \
    "$(echo "$createTaskResult" | jq -r '.status_last_changed')" \
    "$(echo "$createTaskResult" | jq -r '.participants')"
)
getStatus=$(echo "$getTaskResult" | jq -r '.status')

# 執行 list.sh 來查詢工作列表
# listTaskResult=$(bash "$SCRIPT_DIR/list.sh" "$task_id" "$project_id_send" "$task_type_id_send" "$name_send" "$description_send")
# listStatus=$(echo "$listTaskResult" | jq -r '.status')


# 執行 update.sh 來更新工作
updateName="Updated Task Name $(date +%H:%M:%S)"
updateDescription="Updated task description $(date +%H:%M:%S)"
concurrencyStamp=$(echo "$createTaskResult" | jq -r '.concurrency_stamp')
updateTaskResult=$(bash "$SCRIPT_DIR/update.sh" "$task_id" "$another_task_id" "$project_id" "$another_task_type_id" "$another_task_type_name" "$updateName" "$updateDescription" "$concurrencyStamp" "$mentors_id_1" "$mentors_id2" "$executors_id1" "$user_name_1" "$user_name_2" "$user_name_3")
updateStatus=$(echo "$updateTaskResult" | jq -r '.status')

# 執行 delete.sh 來刪除工作
# deleteTaskResult=$(bash "$SCRIPT_DIR/delete.sh" "$task_id")
# deleteStatus=$(echo "$deleteTaskResult" | jq -r '.status')

result_status="success"

if [ "$createStatus" = "fail" ] ; then
    result_status="fail"
    msg=$(echo "$createTaskResult" | jq -r '.msg')
fi

if [ "$getStatus" = "fail" ]; then
    result_status="fail"
    msg=$(echo "$getTaskResult" | jq -r '.msg')
fi

# if [ "$listStatus" = "fail" ]; then
#     result_status="fail"
# fi

if [ "$updateStatus" = "fail" ]; then
    result_status="fail"
    msg=$(echo "$updateTaskResult" | jq -r '.msg')
fi

# if [ "$deleteStatus" = "fail" ]; then
#     result_status="fail"
# fi




result_json=$(jq -n \
    --arg status "$result_status" \
    --arg create "$createStatus" \
    --arg get "$getStatus" \
    --arg update "$updateStatus" \
    --arg msg "$msg" \
    '{status: $status, create: $create, get: $get, update: $update, msg: $msg}')
echo "$result_json"