#!/bin/bash
# 需要傳入 user id, tenant_id, username, email, password 作為參數

# 參數: project_id, tenant_id, name, description
project_id_send="$1"
tenant_id_send="$2"
name_send="$3"
description_send="$4"

if [ -z "$project_id_send" ] || [ -z "$tenant_id_send" ] || [ -z "$name_send" ] || [ -z "$description_send" ]; then
  echo "請依序提供 project_id, tenant_id, name, description 作為參數"
  exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/projects/$project_id_send \
  -H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)


# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
  : # echo "查詢專案成功!"
else
  # echo "查詢專案失敗!"
  exit 1
fi


# 檢查 project 是否存在，並檢查欄位
project_id=$(echo "$body" | jq -r '.project.id')
project_tenant_id=$(echo "$body" | jq -r '.project.tenant_id')
project_name=$(echo "$body" | jq -r '.project.name')
project_description=$(echo "$body" | jq -r '.project.description')
project_concurrency_stamp=$(echo "$body" | jq -r '.project.concurrency_stamp')
project_created_at=$(echo "$body" | jq -r '.project.created_at')

if [ "$project_id" = "$project_id_send" ] && [ "$project_tenant_id" = "$tenant_id_send" ] && [ "$project_name" = "$name_send" ] && [ "$project_description" = "$description_send" ]; then
  result_json=$(jq -n \
    --arg id "$project_id" \
    --arg tenant_id "$project_tenant_id" \
    --arg name "$project_name" \
    --arg description "$project_description" \
    --arg concurrency_stamp "$project_concurrency_stamp" \
    --arg created_at "$project_created_at" \
    --arg status "success" \
    '{status: $status, id: $id, tenant_id: $tenant_id, name: $name, description: $description, concurrency_stamp: $concurrency_stamp, created_at: $created_at}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "查詢失敗、id、tenant_id、name 或 description 不符" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi
