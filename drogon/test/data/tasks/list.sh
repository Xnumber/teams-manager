#!/bin/bash
# 查詢所有 tasks 列表
# 參數: id, project_id, task_type_id, name, description

task_id_send="$1"
project_id_send="$2"
task_type_id_send="$3"
name_send="$4"
description_send="$5"

response=$(curl -s -w "\n%{http_code}" -X GET http://localhost:5001/tasks \
	-H "Content-Type: application/json")
body=$(echo "$response" | head -n -1)
status=$(echo "$response" | tail -n 1)

if [ "$status" -eq 200 ]; then
	result=$(echo "$body" | jq -r '.result')
	tasks=$(echo "$body" | jq -c '.tasks')
	count=$(echo "$body" | jq -r '.count')
	if [ "$result" = "ok" ] && [ -n "$tasks" ] && [ -n "$count" ]; then
		if [ -n "$task_id_send" ] && [ -n "$project_id_send" ] && [ -n "$task_type_id_send" ] && [ -n "$name_send" ] && [ -n "$description_send" ]; then
			filtered_tasks=$(echo "$tasks" | jq -c --arg id "$task_id_send" --arg project "$project_id_send" --arg type "$task_type_id_send" --arg name "$name_send" --arg desc "$description_send" '[.[] | select(.id == $id and .project_id == $project and .task_type_id == $type and .name == $name and .description == $desc)]')
			filtered_count=$(echo "$filtered_tasks" | jq 'length')
			status_val="fail"
			if [ "$filtered_count" -gt 0 ]; then
				status_val="success"
			fi
			result_json=$(jq -n \
				--argjson count "$filtered_count" \
				--arg result "$result" \
				--argjson tasks "$filtered_tasks" \
				--arg status "$status_val" \
				'{status: $status, count: $count, result: $result, tasks: $tasks}')
			echo "$result_json"
		else
			result_json=$(jq -n \
				--argjson count "$count" \
				--arg result "$result" \
				--argjson tasks "$tasks" \
				--arg status "success" \
				'{status: $status, count: $count, result: $result, tasks: $tasks}')
			echo "$result_json"
		fi
	else
		result_json=$(jq -n \
			--arg status "fail" \
			--arg msg "查詢失敗或資料異常" \
			'{status: $status, msg: $msg}')
		echo "$result_json"
	fi
else
	result_json=$(jq -n \
		--arg status "fail" \
		--arg msg "查詢失敗" \
		'{status: $status, msg: $msg}')
	echo "$result_json"
fi
