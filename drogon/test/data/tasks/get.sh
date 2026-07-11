#!/bin/bash
# 查詢單一 task
# 參數: task_id, project_id, task_type_id, name, description

task_id_send="$1"
project_id_send="$2"
task_type_id_send="$3"
name_send="$4"
description_send="$5"
creator_id_send="$6"
creator_name_send="$7"
project_name_send="$8"
task_type_name_send="$9"
remark_send="${10}"
scheduled_start_date_send="${11}"
scheduled_end_date_send="${12}"
completion_date_send="${13}"
progress_send="${14}"
completed_send="${15}"
status_send="${16}"
status_last_changed_send="${17}"
participants_send="${18}"

if [ -z "$task_id_send" ] || [ -z "$project_id_send" ] || [ -z "$task_type_id_send" ] || [ -z "$name_send" ] || [ -z "$description_send" ]; then
  echo "請依序提供 task_id, project_id, task_type_id, name, description, creator_id, creator_name, project_name, task_type_name, remark, scheduled_start_date, scheduled_end_date, completion_date, progress, completed, status, status_last_changed, participants 作為參數"
  exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/tasks/$task_id_send \
  -H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

if [ "$status" -eq 200 ]; then
  :
else
  exit 1
fi

task_id=$(echo "$body" | jq -r '.task.id')
task_project_id=$(echo "$body" | jq -r '.task.project_id')
task_task_type_id=$(echo "$body" | jq -r '.task.task_type_id')
task_name=$(echo "$body" | jq -r '.task.name')
task_description=$(echo "$body" | jq -r '.task.description')
task_creator_id=$(echo "$body" | jq -r '.task.creator_id')
task_creator_name=$(echo "$body" | jq -r '.task.creator_name')
task_project_name=$(echo "$body" | jq -r '.task.project_name')
task_task_type_name=$(echo "$body" | jq -r '.task.task_type_name')
task_remark=$(echo "$body" | jq -r '.task.remark')
task_scheduled_start_date=$(echo "$body" | jq -r '.task.scheduled_start_date')
task_scheduled_end_date=$(echo "$body" | jq -r '.task.scheduled_end_date')
task_completion_date=$(echo "$body" | jq -r '.task.completion_date')
task_progress=$(echo "$body" | jq -r '.task.progress')
task_completed=$(echo "$body" | jq -r '.task.completed')
task_status=$(echo "$body" | jq -r '.task.status')
task_status_last_changed=$(echo "$body" | jq -r '.task.status_last_changed')
task_participants=$(echo "$body" | jq -c '.task.participants')
task_concurrency_stamp=$(echo "$body" | jq -r '.task.concurrency_stamp')
task_created_at=$(echo "$body" | jq -r '.task.created_at')

if [ "$task_id" = "$task_id_send" ] \
  && [ "$task_project_id" = "$project_id_send" ] \
  && [ "$task_task_type_id" = "$task_type_id_send" ] \
  && [ "$task_name" = "$name_send" ] \
  && [ "$task_description" = "$description_send" ] \
  && [ "$task_creator_id" = "$creator_id_send" ] \
  && [ "$task_creator_name" = "$creator_name_send" ] \
  && [ "$task_project_name" = "$project_name_send" ] \
  && [ "$task_task_type_name" = "$task_type_name_send" ] \
  && [ "$task_remark" = "$remark_send" ] \
  && [ "$task_scheduled_start_date" = "$scheduled_start_date_send" ] \
  && [ "$task_scheduled_end_date" = "$scheduled_end_date_send" ] \
  && [ "$task_completion_date" = "$completion_date_send" ] \
  && [ "$task_progress" = "$progress_send" ] \
  && [ "$task_completed" = "$completed_send" ] \
  && [ "$task_status" = "$status_send" ] \
  && [ "$task_status_last_changed" = "$status_last_changed_send" ] \
  && [ "$task_participants" = "$participants_send" ]
then
  result_json=$(jq -n \
    --arg id "$task_id" \
    --arg project_id "$task_project_id" \
    --arg task_type_id "$task_task_type_id" \
    --arg name "$task_name" \
    --arg description "$task_description" \
    --arg creator_id "$task_creator_id" \
    --arg creator_name "$task_creator_name" \
    --arg project_name "$task_project_name" \
    --arg task_type_name "$task_task_type_name" \
    --arg remark "$task_remark" \
    --arg scheduled_start_date "$task_scheduled_start_date" \
    --arg scheduled_end_date "$task_scheduled_end_date" \
    --arg completion_date "$task_completion_date" \
    --arg progress "$task_progress" \
    --arg completed "$task_completed" \
    --arg status "$task_status" \
    --arg status_last_changed "$task_status_last_changed" \
    --arg participants "$task_participants" \
    --arg concurrency_stamp "$task_concurrency_stamp" \
    --arg created_at "$task_created_at" \
    --arg status_result "success" \
    '{status: $status_result, id: $id, project_id: $project_id, task_type_id: $task_type_id, name: $name, description: $description, creator_id: $creator_id, creator_name: $creator_name, project_name: $project_name, task_type_name: $task_type_name, remark: $remark, scheduled_start_date: $scheduled_start_date, scheduled_end_date: $scheduled_end_date, completion_date: $completion_date, progress: $progress, completed: $completed, status: $status, status_last_changed: $status_last_changed, participants: $participants, concurrency_stamp: $concurrency_stamp, created_at: $created_at}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "欄位不匹配" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
fi
