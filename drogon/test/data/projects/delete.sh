
#!/bin/bash


# 參數: project_id
project_id_send="$1"
if [ -z "$project_id_send" ]; then
  echo "請提供 project_id"
  exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X DELETE "http://localhost:5001/projects/$project_id_send" \
  -H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)


if [ "$status" -eq 200 ]; then
  name=$(echo "$body" | jq -r '.project.name')
  description=$(echo "$body" | jq -r '.project.description')
  tenant_id=$(echo "$body" | jq -r '.project.tenant_id')
  id=$(echo "$body" | jq -r '.project.id')
  concurrency_stamp=$(echo "$body" | jq -r '.project.concurrency_stamp')
  created_at=$(echo "$body" | jq -r '.project.created_at')

  # 刪除後再次查詢確認
  get_response=$(curl -s -w "\n%{http_code}" -X GET "http://localhost:5001/projects/$project_id_send" -H "Content-Type: application/json")
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
      --arg tenant_id "$tenant_id" \
      --arg concurrency_stamp "$concurrency_stamp" \
      --arg created_at "$created_at" \
      --arg status "success" \
      '{status: $status, id: $id, name: $name, description: $description, tenant_id: $tenant_id, concurrency_stamp: $concurrency_stamp, created_at: $created_at}')
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
