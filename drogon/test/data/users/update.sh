
#!/bin/bash
user_id_send="$1"
tenant_id_send="${2:-00000000-0000-0000-0000-000000000000}"
username_send="${3:-UpdatedUser}"
email_send="${4:-updateduser@example.com}"
password_send="${5:-updatedpassword}"
concurrency_stamp_send="${6:-$(uuidgen)}"
if [ -z "$user_id_send" ]; then
  echo "請提供 user_id"
  exit 1
fi


API_URL="http://localhost:5001/user/$user_id_send"

update_payload=$(jq -n \
  --arg tenant_id "$tenant_id_send" \
  --arg username "$username_send" \
  --arg email "$email_send" \
  --arg password "$password_send" \
  --arg concurrency_stamp "$concurrency_stamp_send" \
  '{tenant_id: $tenant_id, username: $username, email: $email, password: $password, concurrency_stamp: $concurrency_stamp}')
response=$(curl -s -w "\n%{http_code}" -X PUT "$API_URL" \
  -H "Content-Type: application/json" \
  -d "$update_payload")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)
if [ "$status" -eq 200 ]; then
  : # echo "更新使用者成功!"
else
  # echo "更新使用者失敗!"
  exit 1
fi

username=$(echo "$body" | jq -r '.user.username')
email=$(echo "$body" | jq -r '.user.email')
tenant_id=$(echo "$body" | jq -r '.user.tenant_id')
id=$(echo "$body" | jq -r '.user.id')
response_concurrency_stamp=$(echo "$body" | jq -r '.user.concurrency_stamp')
result=$(echo "$body" | jq -r '.result')

if [ "$username" = "$username_send" ] && [ "$email" = "$email_send" ] && [ "$tenant_id" = "$tenant_id_send" ] && [ "$id" = "$user_id_send" ] && [ "$response_concurrency_stamp" != "$concurrency_stamp_send" ]; then
  # 再次查詢確認資料是否正確
  get_response=$(curl -s -w "\n%{http_code}" -X GET "http://localhost:5001/user/$id" -H "Content-Type: application/json")
  get_body=$(echo "$get_response" | head -n -1)
  get_status=$(echo "$get_response" | tail -n 1)
  get_username=$(echo "$get_body" | jq -r '.user.username')
  get_email=$(echo "$get_body" | jq -r '.user.email')
  get_tenant_id=$(echo "$get_body" | jq -r '.user.tenant_id')
  get_id=$(echo "$get_body" | jq -r '.user.id')
  get_concurrency_stamp=$(echo "$get_body" | jq -r '.user.concurrency_stamp')
  get_result=$(echo "$get_body" | jq -r '.result')
  if [ "$get_status" -eq 200 ] && [ "$get_result" = "ok" ] && [ "$get_id" = "$user_id_send" ] && [ "$get_username" = "$username_send" ] && [ "$get_email" = "$email_send" ] && [ "$get_tenant_id" = "$tenant_id_send" ] && [ "$get_concurrency_stamp" != "$concurrency_stamp_send" ]; then
    result_json=$(jq -n \
      --arg id "$id" \
      --arg username "$username" \
      --arg email "$email" \
      --arg tenant_id "$tenant_id" \
      --arg concurrency_stamp "$response_concurrency_stamp" \
      --arg status "success" \
      '{status: $status, id: $id, username: $username, email: $email, tenant_id: $tenant_id, concurrency_stamp: $concurrency_stamp}')
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