#!/bin/bash
# 刪除 task 測試資料
# 參數: task_id

task_id_send="$1"
if [ -z "$task_id_send" ]; then
  echo "請提供 task_id"
  exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X DELETE "http://localhost:5001/tasks/$task_id_send" \
  -H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

if [ "$status" -eq 200 ]; then
  name=$(echo "$body" | jq -r '.task.name')
  description=$(echo "$body" | jq -r '.task.description')
  project_id=$(echo "$body" | jq -r '.task.project_id')
  task_type_id=$(echo "$body" | jq -r '.task.task_type_id')
  id=$(echo "$body" | jq -r '.task.id')
  concurrency_stamp=$(echo "$body" | jq -r '.task.concurrency_stamp')
  created_at=$(echo "$body" | jq -r '.task.created_at')

  # 刪除後再次查詢確認
  get_response=$(curl -s -w "\n%{http_code}" -X GET "http://localhost:5001/tasks/$task_id_send" -H "Content-Type: application/json")
  get_body=$(echo "$get_response" | head -n -1)
  get_status=$(echo "$get_response" | tail -n 1)

  # 預期查無資料
  msg=$(echo "$get_body" | jq -r '.message // empty')
  result=$(echo "$get_body" | jq -r '.result // empty')
  if [ "$get_status" -ne 200 ] && [ "$msg" = "0 rows found" ] && [ "$result" = "error" ]; then
    result_json=$(jq -n \
      --arg id "$id" \
      --arg name "$name" \
      --arg description "$description" \
      --arg project_id "$project_id" \
      --arg task_type_id "$task_type_id" \
      --arg concurrency_stamp "$concurrency_stamp" \
      --arg created_at "$created_at" \
      --arg status "success" \
      '{status: $status, id: $id, name: $name, description: $description, project_id: $project_id, task_type_id: $task_type_id, concurrency_stamp: $concurrency_stamp, created_at: $created_at}')
    echo "$result_json"
  else
    result_json=$(jq -n \
      --arg status "fail" \
      --arg msg "刪除後查詢仍有資料或回應異常" \
      '{status: $status, msg: $msg}')
    echo "$result_json"
  fi
else
  msg=$(echo "$body" | jq -r '.message // empty')
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "${msg:-刪除失敗}" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
fi
