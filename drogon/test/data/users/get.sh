#!/bin/bash
# 需要傳入 user id, tenant_id, username, email, password 作為參數
user_id_send="$1"
tenant_id_send="$2"
username_send="$3"
email_send="$4"
password_send="$5"

if [ -z "$user_id_send" ] || [ -z "$tenant_id_send" ] || [ -z "$username_send" ] || [ -z "$email_send" ] || [ -z "$password_send" ]; then
  echo "請依序提供 user id, tenant id, username, email, password 作為參數"
  exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/user/$user_id_send \
  -H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
  : # echo "查詢使用者成功!"
else
  # echo "查詢使用者失敗!"
  exit 1
fi

# 檢查 result 是否為 ok，並檢查 user 欄位
result=$(echo "$body" | jq -r '.result')
user_id=$(echo "$body" | jq -r '.user.id')
tenant_id=$(echo "$body" | jq -r '.user.tenant_id')
username=$(echo "$body" | jq -r '.user.username')
email=$(echo "$body" | jq -r '.user.email')
password=$(echo "$body" | jq -r '.user.password')
concurrency_stamp=$(echo "$body" | jq -r '.user.concurrency_stamp')

if [ "$result" = "ok" ] && [ "$user_id" = "$user_id_send" ] && [ "$tenant_id" = "$tenant_id_send" ] && [ "$username" = "$username_send" ] && [ "$email" = "$email_send" ] && [ "$password" = "$password_send" ]; then
  result_json=$(jq -n \
    --arg id "$user_id" \
    --arg tenant "$tenant_id" \
    --arg username "$username" \
    --arg email "$email" \
    --arg password "$password" \
    --arg concurrency_stamp "$concurrency_stamp" \
    --arg status "success" \
    '{status: $status, id: $id, tenant: $tenant, username: $username, email: $email, password: $password, concurrency_stamp: $concurrency_stamp}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "查詢失敗、id、tenant_id、username、email 或 password 不符" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi
