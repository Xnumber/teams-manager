
# 參數: id, tenant_id, name, description
project_id_send="$1"
tenant_id_send="$2"
name_send="$3"
description_send="$4"

# 查詢所有專案列表
response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/projects \
	-H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
	result=$(echo "$body" | jq -r '.result')
	projects=$(echo "$body" | jq -c '.projects')
	count=$(echo "$body" | jq -r '.count')
	if [ "$result" = "ok" ] && [ -n "$projects" ] && [ -n "$count" ]; then
		# 如果有提供 project_id_send, tenant_id_send, name_send, description_send，則過濾
		if [ -n "$project_id_send" ] && [ -n "$tenant_id_send" ] && [ -n "$name_send" ] && [ -n "$description_send" ]; then
			filtered_projects=$(echo "$projects" | jq -c --arg id "$project_id_send" --arg tenant "$tenant_id_send" --arg name "$name_send" --arg desc "$description_send" '[.[] | select(.id == $id and .tenant_id == $tenant and .name == $name and .description == $desc)]')
			filtered_count=$(echo "$filtered_projects" | jq 'length')
			status_val="fail"
			if [ "$filtered_count" -gt 0 ]; then
				status_val="success"
			fi
			result_json=$(jq -n \
				--argjson count "$filtered_count" \
				--arg result "$result" \
				--argjson projects "$filtered_projects" \
				--arg status "$status_val" \
				'{status: $status, count: $count, result: $result, projects: $projects}')
			echo "$result_json"
		else
			result_json=$(jq -n \
				--argjson count "$count" \
				--arg result "$result" \
				--argjson projects "$projects" \
				--arg status "success" \
				'{status: $status, count: $count, result: $result, projects: $projects}')
			echo "$result_json"
		fi
	else
		result_json=$(jq -n \
			--arg result "fail" \
			--arg msg "查詢失敗或資料格式錯誤" \
			'{result: $result, msg: $msg}')
		echo "$result_json"
	fi
else
	result_json=$(jq -n \
		--arg result "fail" \
		--arg msg "HTTP 狀態碼錯誤: $status" \
		'{result: $result, msg: $msg}')
	echo "$result_json"
fi
