#!/bin/bash
tenant_id_send="$1"
tenant_name_send="${2:-Updated Tenant}"
tenant_desc_send="${3:-Updated description}"
if [ -z "$tenant_id_send" ]; then
  echo "請提供 tenant_id"
  exit 1
fi
response=$(curl -s -w "\n%{http_code}" -X PUT "http://localhost:5001/tenant/$tenant_id_send" \
  -H "Content-Type: application/json" \
  -d "{\"name\": \"$tenant_name_send\", \"description\": \"$tenant_desc_send\"}")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)
if [ "$status" -eq 200 ]; then
  : # echo "更新租戶成功!"
else
  # echo "更新租戶失敗!"
  exit 1
fi
tenant_name=$(echo "$body" | jq -r '.tenant.name')
tenant_desc=$(echo "$body" | jq -r '.tenant.description')
tenant_id=$(echo "$body" | jq -r '.tenant.id')
if [ "$tenant_name" = "$tenant_name_send" ] && [ "$tenant_desc" = "$tenant_desc_send" ]; then
  # 再次查詢確認資料是否正確
  get_response=$(curl -s -w "\n%{http_code}" -X GET "http://localhost:5001/tenant/$tenant_id" -H "Content-Type: application/json")
  get_body=$(echo "$get_response" | head -n -1)
  get_status=$(echo "$get_response" | tail -n 1)
  get_name=$(echo "$get_body" | jq -r '.tenant.name')
  get_desc=$(echo "$get_body" | jq -r '.tenant.description')
  get_id=$(echo "$get_body" | jq -r '.tenant.id')
  get_result=$(echo "$get_body" | jq -r '.result')
  if [ "$get_status" -eq 200 ] && [ "$get_result" = "ok" ] && [ "$get_id" = "$tenant_id_send" ] && [ "$get_name" = "$tenant_name_send" ] && [ "$get_desc" = "$tenant_desc_send" ]; then
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
      --arg msg "更新後查詢資料不符" \
      '{status: $status, msg: $msg}')
    echo "$result_json"
    exit 1
  fi
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "Name 或 Description 不匹配" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi