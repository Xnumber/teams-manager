tenant_id_send="$1"
tenant_name_send="$2"
tenant_desc_send="$3"
# 查詢所有租戶列表
response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/tenant \
	-H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
	result=$(echo "$body" | jq -r '.result')
	tenants=$(echo "$body" | jq -c '.tenants')
	count=$(echo "$body" | jq -r '.count')
	if [ "$result" = "ok" ] && [ -n "$tenants" ] && [ -n "$count" ]; then
		# 如果有提供 tenant_id_send, tenant_name_send, tenant_desc_send，則過濾
		if [ -n "$tenant_id_send" ] && [ -n "$tenant_name_send" ] && [ -n "$tenant_desc_send" ]; then
			filtered_tenants=$(echo "$tenants" | jq -c --arg id "$tenant_id_send" --arg name "$tenant_name_send" --arg desc "$tenant_desc_send" '[.[] | select(.id == $id and .name == $name and .description == $desc)]')
			filtered_count=$(echo "$filtered_tenants" | jq 'length')
			status_val="fail"
			if [ "$filtered_count" -gt 0 ]; then
				status_val="success"
			fi
			result_json=$(jq -n \
				--argjson count "$filtered_count" \
				--arg result "$result" \
				--argjson tenants "$filtered_tenants" \
				--arg status "$status_val" \
				'{status: $status, count: $count, result: $result, tenants: $tenants}')
			echo "$result_json"
		else
			result_json=$(jq -n \
				--argjson count "$count" \
				--arg result "$result" \
				--argjson tenants "$tenants" \
				--arg status "success" \
				'{status: $status, count: $count, result: $result, tenants: $tenants}')
			echo "$result_json"
		fi
	else
		result_json=$(jq -n \
			--arg result "fail" \
			--arg msg "查詢失敗或資料格式錯誤" \
			'{result: $result, msg: $msg}')
		echo "$result_json"
		exit 1
	fi
else
	result_json=$(jq -n \
		--arg result "fail" \
		--arg msg "HTTP 狀態碼錯誤: $status" \
		'{result: $result, msg: $msg}')
	echo "$result_json"
	exit 1
fi
