#!/bin/bash
# 工作更新測試腳本
# 參數: task_id, project_id, task_type_id, name, description, concurrency_stamp


task_id_send="$1"
another_task_id_send="$2"
project_id_send="${3:-}"
task_type_id_send="${4:-}"
task_type_name_to_check="${5:-}"
name_send="${6:-UpdatedTask}"
description_send="${7:-updated task description}"
concurrency_stamp_send="${8:-$(uuidgen)}"
mentors_id_1="${9:-}"
mentors_id_2="${10:-}"
executors_id_1="${11:-}"
mentors_name_1="${12:-}"
mentors_name_2="${13:-}"
executors_name_1="${14:-}"
if [ -z "$task_id_send" ]; then
  echo "請提供 task_id 作為參數。"
  exit 1
fi

API_URL_1="http://localhost:5001/tasks/$task_id_send"
API_URL_2="http://localhost:5001/tasks/$another_task_id_send"

this_week_update_payload=$(jq -n \
  --arg project_id "$project_id_send" \
  --arg task_type_id "$task_type_id_send" \
  --arg name "$name_send" \
  --arg description "$description_send" \
  --arg concurrency_stamp "$concurrency_stamp_send" \
  '{project_id: $project_id, "completed": false, "this_week": true, task_type_id: $task_type_id, name: $name, description: $description, concurrency_stamp: $concurrency_stamp, mentorIds: ["'"$mentors_id_1"'"], executorIds: ["'"$executors_id_1"'"]}')

# 本周工作的 API 呼叫放在這裡，確保本周工作成功後才執行
this_week_response=$(curl -s -w "\n%{http_code}" -X PUT "$API_URL_1" \
  -H "Content-Type: application/json" \
  -d "$this_week_update_payload")
this_week_body=$(echo "$this_week_response" | head -n -1)
status=$(echo "$this_week_response" | tail -n 1)

if [ "$status" -eq 200 ]; then
  :
else
   result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "更新工作錯誤: $this_week_body" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi

task_id=$(echo "$this_week_body" | jq -r '.task.id')
task_project_id=$(echo "$this_week_body" | jq -r '.task.project_id')
task_task_type_id=$(echo "$this_week_body" | jq -r '.task.task_type_id')
task_name=$(echo "$this_week_body" | jq -r '.task.name')
task_description=$(echo "$this_week_body" | jq -r '.task.description')
task_concurrency_stamp=$(echo "$this_week_body" | jq -r '.task.concurrency_stamp')
task_created_at=$(echo "$this_week_body" | jq -r '.task.created_at')
task_task_type_name=$(echo "$this_week_body" | jq -r '.task.task_type_name')
# 檢查本週工作的 scheduled_start_date 和 scheduled_end_date 是否為本週一和週五
task_week_start=$(echo "$this_week_body" | jq -r '.task.scheduled_start_date')
task_week_end=$(echo "$this_week_body" | jq -r '.task.scheduled_end_date')
# 計算本週一與週五
today=$(date +%Y-%m-%d)
wday=$(date -d "$today" +%u)
monday=$(date -d "$today -$((wday-1)) days" +%Y-%m-%d)
friday=$(date -d "$monday +4 days" +%Y-%m-%d)

# 檢查 日期是否在本週範圍內
if [ "$task_week_start" = "$monday" ] && [ "$task_week_end" = "$friday" ]; then
  :
else
  result_json=$(jq -n \
  --arg status "fail" \
  --arg msg "新增本週工作的 scheduled_start_date 或 scheduled_end_date 不在本週範圍內 $task_week_start - $task_week_end" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi




# 檢查 mentors/executors id 與 name 配對
# task_participants=$(echo "$this_week_body" | jq -r '.task.participants')
# if [ "$task_participants" != "null" ]; then
#   participants_json=$(echo "$task_participants" | jq -c .)
#   mentor1_match=$(echo "$participants_json" | jq -c --arg id "$mentors_id_1" --arg name "$mentors_name_1" '.[] | select(.id==$id and .name==$name and .type=="mentor")')
#   executor_match=$(echo "$participants_json" | jq -c --arg id "$executors_id_1" --arg name "$executors_name_1" '.[] | select(.id==$id and .name==$name and .type=="executor")')
#   if [ -n "$mentor1_match" ] && [ -n "$executor_match" ]; then
#     :
#   else
#     result_json=$(jq -n \
#       --arg status "fail" \
#       --arg msg "participants 配對失敗: mentors/executors id/name 不正確" \
#       '{status: $status, msg: $msg}')
#     echo "$result_json"
#     exit 1
#   fi
# else 
#   result_json=$(jq -n \
#     --arg status "fail" \
#     --arg msg "participants 為 null" \
#     '{status: $status, msg: $msg}')
#   echo "$result_json"
#   exit 1
# fi
# 檢查下週工作的 scheduled_start_date 和 scheduled_end_date 是否為下週一和週五
next_week_update_payload=$(jq -n \
  --arg project_id "$project_id_send" \
  --arg task_type_id "$task_type_id_send" \
  --arg name "$name_send" \
  --arg description "$description_send" \
  --arg concurrency_stamp "$concurrency_stamp_send" \
  '{project_id: $project_id, "completed": true, "this_week": false, task_type_id: $task_type_id, name: $name, description: $description, concurrency_stamp: $concurrency_stamp, mentorIds: ["'"$mentors_id_1"'", "'"$mentors_id_2"'"], executorIds: ["'"$executors_id_1"'"]}')

next_week_response=$(curl -s -w "\n%{http_code}" -X PUT "$API_URL_2" \
  -H "Content-Type: application/json" \
  -d "$next_week_update_payload")
next_week_body=$(echo "$next_week_response" | head -n -1)

next_task_week_start=$(echo "$next_week_body" | jq -r '.task.scheduled_start_date')
next_task_week_end=$(echo "$next_week_body" | jq -r '.task.scheduled_end_date')
# 計算下週一與週五
today=$(date +%Y-%m-%d)
wday=$(date -d "$today" +%u)
monday=$(date -d "$today -$((wday-1)) days +7 days" +%Y-%m-%d)
friday=$(date -d "$monday +4 days" +%Y-%m-%d)

if [ "$next_task_week_start" = "$monday" ] && [ "$next_task_week_end" = "$friday" ]; then
  :
else
  result_json=$(jq -n \
  --arg status "fail" \
  --arg msg "新增下週工作的 scheduled_start_date 或 scheduled_end_date 不在下週範圍內 $next_task_week_start - $next_task_week_end, $next_week_body" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi

if [ "$task_id" = "$task_id_send" ] && [ "$task_project_id" = "$project_id_send" ] && [ "$task_task_type_id" = "$task_type_id_send" ] && [ "$task_task_type_name" = "$task_type_name_to_check" ] && [ "$task_name" = "$name_send" ] && [ "$task_description" = "$description_send" ]; then
  result_json=$(jq -n \
    --arg id "$task_id" \
    --arg project_id "$task_project_id" \
    --arg task_type_id "$task_task_type_id" \
    --arg task_type_name "$task_task_type_name" \
    --arg name "$task_name" \
    --arg description "$task_description" \
    --arg concurrency_stamp "$task_concurrency_stamp" \
    --arg created_at "$task_created_at" \
    --arg status "success" \
    '{status: $status, id: $id, project_id: $project_id, task_type_id: $task_type_id, task_type_name: $task_type_name, name: $name, description: $description, concurrency_stamp: $concurrency_stamp, created_at: $created_at}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "欄位不匹配" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
fi
