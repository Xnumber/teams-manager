# 這個腳本用於執行功能測試

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"


# 租戶相關測試
tenant_name_send="New Tenant"
update_tenant_name_send="Updated Tenant"
update_tenant_desc_send="Updated description"
tenant_desc_send="Description of the new tenant"

# 創建租戶 
createResult=$(bash "$SCRIPT_DIR/tenants/create.sh" "$tenant_name_send" "$tenant_desc_send")
tenant_id=$(echo "$createResult" | jq -r '.id')

if [ -n "$tenant_id" ] && [ "$tenant_id" != "null" ]; then
    echo "✅ 租戶創建成功，ID: $tenant_id。"
else
    echo "❌ 租戶創建失敗，無法獲取有效的 tenant ID。"
    echo "創建結果: $createResult"
    exit 1
fi


# 查詢租戶
getResult=$(bash "$SCRIPT_DIR/tenants/get.sh" "$tenant_id" "$tenant_name_send" "$tenant_desc_send")
if echo "$getResult" | jq -e '.status == "success"' > /dev/null; then
    echo "✅ 租戶查詢成功，資料匹配。"
else
    echo "❌ 租戶查詢失敗，資料不匹配。"
    echo "查詢結果: $getResult"
    exit 1
fi

# 查詢租戶列表
listResult=$(bash "$SCRIPT_DIR/tenants/list.sh" "$tenant_id" "$tenant_name_send" "$tenant_desc_send")
if echo "$listResult" | jq -e '.status == "success" and .count == 1' > /dev/null; then
    echo "✅ 租戶列表查詢成功，資料匹配。"
else
    echo "❌ 租戶列表查詢失敗，資料不匹配。"
    echo "查詢結果: $listResult"
    exit 1
fi

# 更新租戶
updateResult=$(bash "$SCRIPT_DIR/tenants/update.sh" "$tenant_id" "$update_tenant_name_send" "$update_tenant_desc_send")
if echo "$updateResult" | jq -e '.status == "success"' > /dev/null; then
    echo "✅ 租戶更新成功，資料匹配。"
else
    echo "❌ 租戶更新失敗，資料不匹配。"
    echo "更新結果: $updateResult"
    exit 1
fi


# 使用者資料測試
userTestResult=$(bash "$SCRIPT_DIR/users/execute.sh" "$tenant_id")
if echo "$userTestResult" | jq -e '.status == "success"' > /dev/null; then
    echo "✅ 使用者資料測試通過。"
else
    echo "❌ 使用者資料測試失敗。"
    echo "測試結果: $userTestResult"
    exit 1
fi

# 專案資料測試
projectName="Test Project"
projectDescription="This is a test project."

projectTestResult=$(bash "$SCRIPT_DIR/projects/execute.sh" "$tenant_id" "$projectName" "$projectDescription")
if echo "$projectTestResult" | jq -e '.status == "success"' > /dev/null; then
    echo "✅ 專案資料測試通過。"
else
    echo "❌ 專案資料測試失敗。"
    echo "測試結果: $projectTestResult"
    exit 1
fi

# 工作資料測試
taskResult=$(bash "$SCRIPT_DIR/tasks/execute.sh")
if echo "$taskResult" | jq -e '.status == "success"' > /dev/null; then
    echo "✅ 工作資料測試通過。"
else
    echo "❌ 工作資料測試失敗。"
    echo "測試結果: $taskResult"
    exit 1
fi


# 刪除租戶
# deleteResult=$(bash "$SCRIPT_DIR/tenants/delete.sh" "$tenant_id")
# if echo "$deleteResult" | jq -e '.status == "success"' > /dev/null; then
#     echo "✅ 租戶刪除成功，資料匹配。"
# else
#     echo "❌ 租戶刪除失敗，資料不匹配。"
#     echo "刪除結果: $deleteResult"
#     exit 1
# fi