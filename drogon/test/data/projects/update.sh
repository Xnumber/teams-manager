
#!/bin/bash
# 專案更新測試腳本，參考 create_projects_table.sh 與 create.sh
# 參數: project_id, tenant_id, name, description, concurrency_stamp
project_id_send="$1"
tenant_id_send="${2:-}"
name_send="${3:-UpdatedProject}"
description_send="${4:-updated project description}"
concurrency_stamp_send="${5:-$(uuidgen)}"

if [ -z "$project_id_send" ]; then
  echo "請提供 project_id 作為參數。"
  exit 1
fi

API_URL="http://localhost:5001/projects/$project_id_send"

update_payload=$(jq -n \
  --arg tenant_id "$tenant_id_send" \
  --arg name "$name_send" \
  --arg description "$description_send" \
  --arg concurrency_stamp "$concurrency_stamp_send" \
  '{tenant_id: $tenant_id, name: $name, description: $description, concurrency_stamp: $concurrency_stamp}')

response=$(curl -s -w "\n%{http_code}" -X PUT "$API_URL" \
  -H "Content-Type: application/json" \
  -d "$update_payload")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

if [ "$status" -eq 200 ]; then
  : # echo "更新專案成功!"
else
   result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "更新專案錯誤" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi

# 檢查 project 是否存在，並檢查欄位
project_id=$(echo "$body" | jq -r '.project.id')
project_tenant_id=$(echo "$body" | jq -r '.project.tenant_id')
project_name=$(echo "$body" | jq -r '.project.name')
project_description=$(echo "$body" | jq -r '.project.description')
project_concurrency_stamp=$(echo "$body" | jq -r '.project.concurrency_stamp')
result=$(echo "$body" | jq -r '.result')

if [ "$project_tenant_id" = "$tenant_id_send" ] && [ "$project_name" = "$name_send" ] && [ "$project_description" = "$description_send" ] && [ "$project_id" = "$project_id_send" ] && [ "$project_concurrency_stamp" != "$concurrency_stamp_send" ]; then
  # 再次查詢確認資料是否正確
  get_response=$(curl -s -w "\n%{http_code}" -X GET "http://localhost:5001/projects/$project_id" -H "Content-Type: application/json")
  get_body=$(echo "$get_response" | head -n -1)
  get_status=$(echo "$get_response" | tail -n 1)
  get_project_id=$(echo "$get_body" | jq -r '.project.id')
  get_project_tenant_id=$(echo "$get_body" | jq -r '.project.tenant_id')
  get_project_name=$(echo "$get_body" | jq -r '.project.name')
  get_project_description=$(echo "$get_body" | jq -r '.project.description')
  get_project_concurrency_stamp=$(echo "$get_body" | jq -r '.project.concurrency_stamp')
  get_result=$(echo "$get_body" | jq -r '.result')
  if [ "$get_status" -eq 200 ] && [ "$get_result" = "ok" ] && [ "$get_project_id" = "$project_id_send" ] && [ "$get_project_tenant_id" = "$tenant_id_send" ] && [ "$get_project_name" = "$name_send" ] && [ "$get_project_description" = "$description_send" ] && [ "$get_project_concurrency_stamp" != "$concurrency_stamp_send" ]; then
    result_json=$(jq -n \
      --arg id "$project_id" \
      --arg tenant_id "$project_tenant_id" \
      --arg name "$project_name" \
      --arg description "$project_description" \
      --arg concurrency_stamp "$project_concurrency_stamp" \
      --arg status "success" \
      '{status: $status, id: $id, tenant_id: $tenant_id, name: $name, description: $description, concurrency_stamp: $concurrency_stamp}')
    echo "$result_json"
  else
    result_json=$(jq -n \
      --arg status "fail" \
      --arg msg "更新後查詢資料不符" \
      '{status: $status, msg: $msg}')
    echo "$result_json"
    exit 1
  fi
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "欄位資料不匹配" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi