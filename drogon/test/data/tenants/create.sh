#!/bin/bash
tenant_name_send="${1:-New Tenant}"
tenant_desc_send="${2:-Description of the new tenant}"
response=$(curl -s -w "\n%{http_code}" -X POST http://localhost:5001/tenant \
  -H "Content-Type: application/json" \
  -d "{\"name\": \"$tenant_name_send\", \"description\": \"$tenant_desc_send\"}") 
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# echo "Body: $body"
# echo "Status: $status"
# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
  : # echo "新增租戶成功!"
else
  # echo "新增租戶失敗!"
  exit 1
fi
# 檢查 tenant 是否存在，並檢查 name 和 description
tenant_name=$(echo "$body" | jq -r '.tenant.name')
tenant_desc=$(echo "$body" | jq -r '.tenant.description')
tenant_id=$(echo "$body" | jq -r '.tenant.id')
if [ "$tenant_name" = "$tenant_name_send" ] && [ "$tenant_desc" = "$tenant_desc_send" ]; then
  result_json=$(jq -n \
    --arg id "$tenant_id" \
    --arg name "$tenant_name" \
    --arg desc "$tenant_desc" \
    --arg status "success" \
    '{status: $status, id: $id, name: $name, desc: $desc}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "Name 或 Description 不匹配" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  # exit 1
fi