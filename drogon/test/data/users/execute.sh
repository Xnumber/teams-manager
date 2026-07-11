tenant_id_send="${1:-}"
# echo "$result_json"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

tenant_id_send="${1:-}"
username_send="testuser"
email_send="testuser@example.com"
password_send="testpassword"


# 執行 create.sh 來創建使用者
createUserResult=$(bash "$SCRIPT_DIR/create.sh" "$tenant_id_send" "$username_send" "$email_send" "$password_send")
createStatus=$(echo "$createUserResult" | jq -r '.status')
# 測試取單一使用者
userId=$(echo "$createUserResult" | jq -r '.id')
getUserResult=$(bash "$SCRIPT_DIR/get.sh" "$userId" "$tenant_id_send" "$username_send" "$email_send" "$password_send")
getStatus=$(echo "$getUserResult" | jq -r '.status')
concurrencyStamp=$(echo "$getUserResult" | jq -r '.concurrency_stamp')
# 測試取使用者列表
listUserResult=$(bash "$SCRIPT_DIR/list.sh" "$userId" "$tenant_id_send" "$username_send" "$email_send")
listStatus=$(echo "$listUserResult" | jq -r '.status')
# 測試更新使用者
updateUserResult=$(bash "$SCRIPT_DIR/update.sh" "$userId" "$tenant_id_send" "updateduser" "updateduser@example.com" "updatedpassword" "$concurrencyStamp")

updateStatus=$(echo "$updateUserResult" | jq -r '.status')

# 測試刪除使用者
deleteUserResult=$(bash "$SCRIPT_DIR/delete.sh" "$userId")
deleteStatus=$(echo "$deleteUserResult" | jq -r '.status')

result_status="success"

if [ "$createStatus" = "fail" ]; then
    result_status="fail"
fi

if [ "$getStatus" = "fail" ]; then
    result_status="fail"
fi

if [ "$listStatus" = "fail" ]; then
    result_status="fail"
fi

if [ "$updateStatus" = "fail" ]; then
    result_status="fail"
fi



if [ "$deleteStatus" = "fail" ]; then
    result_status="fail"
fi

result_json=$(jq -n \
        --arg status "$result_status" \
        --arg create "$createStatus" \
        --arg get "$getStatus" \
        --arg list "$listStatus" \
        --arg update "$updateStatus" \
        --arg delete "$deleteStatus" \
        '{ status: $status, create: $create, get: $get, list: $list, update: $update, delete: $delete }')

echo "$result_json"