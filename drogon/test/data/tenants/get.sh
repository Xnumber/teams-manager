#!/bin/bash
# 需要傳入 tenant id、tenant_name_send、tenant_desc_send 作為參數
tenant_id_send="$1"
tenant_name_send="$2"
tenant_desc_send="$3"

if [ -z "$tenant_id_send" ] || [ -z "$tenant_name_send" ] || [ -z "$tenant_desc_send" ]; then
	echo "請依序提供 tenant id, tenant name, tenant description 作為參數"
	exit 1
fi

response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/tenant/$tenant_id_send \
	-H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
	: # echo "查詢租戶成功!"
else
	# echo "查詢租戶失敗!"
	exit 1
fi

# 檢查 result 是否為 ok，並檢查 tenant 欄位
result=$(echo "$body" | jq -r '.result')
tenant_id=$(echo "$body" | jq -r '.tenant.id')
tenant_name=$(echo "$body" | jq -r '.tenant.name')
tenant_desc=$(echo "$body" | jq -r '.tenant.description')

if [ "$result" = "ok" ] && [ "$tenant_id" = "$tenant_id_send" ] && [ "$tenant_name" = "$tenant_name_send" ] && [ "$tenant_desc" = "$tenant_desc_send" ]; then
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
		--arg msg "查詢失敗、id、name 或 description 不符" \
		'{status: $status, msg: $msg}')
	echo "$result_json"
	exit 1
fi
