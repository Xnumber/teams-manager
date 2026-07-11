user_id_send="$1"
tenant_id_send="$2"
username_send="$3"
email_send="$4"
# 查詢所有使用者列表
response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/user \
	-H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

# 檢查 HTTP 狀態碼是否為 200
if [ "$status" -eq 200 ]; then
	result=$(echo "$body" | jq -r '.result')
	users=$(echo "$body" | jq -c '.users')
	count=$(echo "$body" | jq -r '.count')
	if [ "$result" = "ok" ] && [ -n "$users" ] && [ -n "$count" ]; then
		# 如果有提供 user_id_send, tenant_id_send, username_send, email_send，則過濾
		if [ -n "$user_id_send" ] && [ -n "$tenant_id_send" ] && [ -n "$username_send" ] && [ -n "$email_send" ]; then
			filtered_users=$(echo "$users" | jq -c --arg id "$user_id_send" --arg tenant "$tenant_id_send" --arg username "$username_send" --arg email "$email_send" '[.[] | select(.id == $id and .tenant_id == $tenant and .username == $username and .email == $email)]')
			filtered_count=$(echo "$filtered_users" | jq 'length')
			status_val="fail"
			if [ "$filtered_count" -gt 0 ]; then
				status_val="success"
			fi
			result_json=$(jq -n \
				--argjson count "$filtered_count" \
				--arg result "$result" \
				--argjson users "$filtered_users" \
				--arg status "$status_val" \
				'{status: $status, count: $count, result: $result, users: $users}')
			echo "$result_json"
		else
			result_json=$(jq -n \
				--argjson count "$count" \
				--arg result "$result" \
				--argjson users "$users" \
				--arg status "success" \
				'{status: $status, count: $count, result: $result, users: $users}')
			echo "$result_json"
		fi
	else
		result_json=$(jq -n \
			--arg result "fail" \
			--arg msg "查詢失敗或資料格式錯誤" \
			'{result: $result, msg: $msg}')
		echo "$result_json"
		# exit 1
	fi
else
	result_json=$(jq -n \
		--arg result "fail" \
		--arg msg "HTTP 狀態碼錯誤: $status" \
		'{result: $result, msg: $msg}')
	echo "$result_json"
	# exit 1
fi
