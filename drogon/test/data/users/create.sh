#!/bin/bash
# 建立 user 測試資料 (參考 tenants 的 create.sh)
API_URL="http://localhost:5001/user"

# 參數: tenant_id, username, email, password
tenant_id_send="${1:-}"
username_send="${2:-testuser}"
email_send="${3:-testuser@example.com}"
password_send="${4:-testpassword}"

if [ -z "$tenant_id_send" ]; then
  echo "請提供 tenant_id 作為參數。"
  exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -d "{\"tenant_id\": \"$tenant_id_send\", \"username\": \"$username_send\", \"email\": \"$email_send\", \"password\": \"$password_send\"}")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
  : # echo "新增使用者成功!"
else
  echo -e "新增使用者失敗!"
  exit 1
fi



# 檢查 user 是否存在，並檢查欄位
user_id=$(echo "$body" | jq -r '.user.id')
user_tenant_id=$(echo "$body" | jq -r '.user.tenant_id')
user_username=$(echo "$body" | jq -r '.user.username')
user_email=$(echo "$body" | jq -r '.user.email')
user_password=$(echo "$body" | jq -r '.user.password')
user_concurrency_stamp=$(echo "$body" | jq -r '.user.concurrency_stamp')
user_created_at=$(echo "$body" | jq -r '.user.created_at')

if [ "$user_tenant_id" = "$tenant_id_send" ] && [ "$user_username" = "$username_send" ] && [ "$user_email" = "$email_send" ]; then
  result_json=$(jq -n \
    --arg id "$user_id" \
    --arg tenant_id "$user_tenant_id" \
    --arg username "$user_username" \
    --arg email "$user_email" \
    --arg password "$user_password" \
    --arg concurrency_stamp "$user_concurrency_stamp" \
    --arg created_at "$user_created_at" \
    --arg status "success" \
    '{status: $status, id: $id, tenant_id: $tenant_id, username: $username, email: $email, password: $password, concurrency_stamp: $concurrency_stamp, created_at: $created_at}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "欄位不匹配" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  # exit 1
fi
