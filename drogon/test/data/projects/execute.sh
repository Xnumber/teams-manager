tenant_id_send="${1:-}"
name_send="${2:-testproject}"
description_send="${3:-test project description}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 執行 create.sh 來創建專案
createProjectResult=$(bash "$SCRIPT_DIR/create.sh" "$tenant_id_send" "$name_send" "$description_send")
createStatus=$(echo "$createProjectResult" | jq -r '.status')
project_id=$(echo "$createProjectResult" | jq -r '.id')

# 執行 get.sh 來查詢專案
getProjectResult=$(bash "$SCRIPT_DIR/get.sh" "$project_id" "$tenant_id_send" "$name_send" "$description_send")
getStatus=$(echo "$getProjectResult" | jq -r '.status')

# 執行 list.sh 來查詢專案列表
listProjectResult=$(bash "$SCRIPT_DIR/list.sh" "$project_id" "$tenant_id_send" "$name_send" "$description_send")
listStatus=$(echo "$listProjectResult" | jq -r '.status')


# 執行 update.sh 來更新專案
updateName="Updated Project Name"
udpateDescription="Updated project description"
concurrencyStamp=$(echo "$createProjectResult" | jq -r '.concurrency_stamp')

updateProjectResult=$(bash "$SCRIPT_DIR/update.sh" "$project_id" "$tenant_id_send" "$updateName" "$udpateDescription" "$concurrencyStamp")
updateStatus=$(echo "$updateProjectResult" | jq -r '.status')

# 執行 delete.sh 來刪除專案
deleteProjectResult=$(bash "$SCRIPT_DIR/delete.sh" "$project_id")
deleteStatus=$(echo "$deleteProjectResult" | jq -r '.status')

result_status="success"

if [ "$createStatus" = "fail" ] ; then
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
        '{ status: $status, create: $create, get: $get, list: $list, update: $update }')

echo "$result_json"