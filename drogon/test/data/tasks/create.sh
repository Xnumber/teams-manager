API_URL="http://localhost:5001/tasks"



# 參數: project_id, task_type_id, name, description
project_id_send="${1:-}"
task_type_id_send="${2:-}"
# 新增 mentorsId 與 executors 支援
mentors_id1="${3:-}"
mentors_id2="${4:-}"
executors_id1="${5:-}"
mentors_name1="${6:-}"
mentors_name2="${7:-}"
executors_name1="${8:-}"
name_send="testtask"
description_send="test task description"
remark_send="test task remark"

if [ -z "$project_id_send" ]; then
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "請提供 project_id" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi

# 本週工作的 API 呼叫放在這裡，確保本週工作成功後才執行
this_week_response=$(curl -s -w "\n%{http_code}" -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -d "{\"project_id\": \"$project_id_send\", \"this_week\": true, \"creator_id\": \"$executors_id1\", \"task_type_id\": \"$task_type_id_send\", \"name\": \"$name_send\", \"description\": \"$description_send\", \"remark\": \"$remark_send\", \"mentorIds\": [\"$mentors_id1\", \"$mentors_id2\"], \"executorIds\": [\"$executors_id1\"]}")
body=$(echo "$this_week_response" | head -n -1)
status=$(echo "$this_week_response" | tail -n 1)
# 下周工作的 API 呼叫放在這裡，確保本週工作成功後才執行
next_week_response=$(curl -s -w "\n%{http_code}" -X POST "$API_URL" \
  -H "Content-Type: application/json" \
  -d "{\"project_id\": \"$project_id_send\", \"this_week\": false, \"creator_id\": \"$executors_id1\", \"task_type_id\": \"$task_type_id_send\", \"name\": \"$name_send\", \"description\": \"$description_send\", \"remark\": \"$remark_send\", \"mentorIds\": [\"$mentors_id1\", \"$mentors_id2\"], \"executorIds\": [\"$executors_id1\"]}")
next_week_body=$(echo "$next_week_response" | head -n -1)
next_week_status=$(echo "$next_week_response" | tail -n 1)
if [ "$next_week_status" -eq 200 ]; then
  :
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "新增下周工作失敗" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi

if [ "$status" -eq 200 ]; then
  :
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "新增本週工作失敗" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi
# 檢查本週工作的 scheduled_start_date 和 scheduled_end_date 是否為本週一和週五
task_week_start=$(echo "$body" | jq -r '.task.scheduled_start_date')
task_week_end=$(echo "$body" | jq -r '.task.scheduled_end_date')
# 計算本週一與週五
today=$(date +%Y-%m-%d)
wday=$(date -d "$today" +%u)
monday=$(date -d "$today -$((wday-1)) days" +%Y-%m-%d)
friday=$(date -d "$monday +4 days" +%Y-%m-%d)
if [ "$task_week_start" = "$monday" ] && [ "$task_week_end" = "$friday" ]; then
  :
else
  result_json=$(jq -n \
  --arg status "fail" \
  --arg msg "新增本週工作的 scheduled_start_date 或 scheduled_end_date 不在本週範圍內" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi
# 檢查下週工作的 scheduled_start_date 和 scheduled_end_date 是否為下週一和週五
task_next_week_start=$(echo "$next_week_body" | jq -r '.task.scheduled_start_date')
task_next_week_end=$(echo "$next_week_body" | jq -r '.task.scheduled_end_date')
# 計算下週一與週五
today=$(date +%Y-%m-%d)
wday=$(date -d "$today" +%u)
next_monday=$(date -d "$today -$((wday-1)) days +7 days" +%Y-%m-%d)
next_friday=$(date -d "$next_monday +4 days" +%Y-%m-%d)
if [ "$task_next_week_start" = "$next_monday" ] && [ "$task_next_week_end" = "$next_friday" ]; then
  :
else
  result_json=$(jq -n \
  --arg status "fail" \
  --arg msg "新增下週工作的 scheduled_start_date 或 scheduled_end_date 不在下週範圍內" \
    '{status: $status, msg: $msg}')
  echo "$result_json"
  exit 1
fi
# 檢查 mentors/executors id 與 name 配對
# task_participants=$(echo "$body" | jq -r '.task.participants')
# if [ "$task_participants" != "null" ]; then
#   participants_json=$(echo "$task_participants" | jq -c .)
#   mentor1_match=$(echo "$participants_json" | jq -c --arg id "$mentors_id1" --arg name "$mentors_name1" '.[] | select(.id==$id and .name==$name and .type=="mentor")')
#   mentor2_match=$(echo "$participants_json" | jq -c --arg id "$mentors_id2" --arg name "$mentors_name2" '.[] | select(.id==$id and .name==$name and .type=="mentor")')
#   executor_match=$(echo "$participants_json" | jq -c --arg id "$executors_id1" --arg name "$executors_name1" '.[] | select(.id==$id and .name==$name and .type=="executor")')
#   if [ -n "$mentor1_match" ] && [ -n "$mentor2_match" ] && [ -n "$executor_match" ]; then
#     :
#   else
#     result_json=$(jq -n \
#       --arg status "fail" \
#       --arg msg "participants 配對失敗: mentors/executors id/name 不正確" \
#       '{status: $status, msg: $msg}')
#     echo "$result_json"
#     exit 1
#   fi
# else 
#   result_json=$(jq -n \
#     --arg status "fail" \
#     --arg msg "participants 為 null" \
#     '{status: $status, msg: $msg}')
#   echo "$result_json"
#   exit 1
# fi

task_id=$(echo "$body" | jq -r '.task.id')
task_project_id=$(echo "$body" | jq -r '.task.project_id')
task_task_type_id=$(echo "$body" | jq -r '.task.task_type_id')
another_task_id=$(echo "$next_week_body" | jq -r '.task.id')
task_name=$(echo "$body" | jq -r '.task.name')
task_description=$(echo "$body" | jq -r '.task.description')
task_concurrency_stamp=$(echo "$body" | jq -r '.task.concurrency_stamp')
task_created_at=$(echo "$body" | jq -r '.task.created_at')
task_remark=$(echo "$body" | jq -r '.task.remark')
task_creator_id=$(echo "$body" | jq -r '.task.creator_id')
task_creator_name=$(echo "$body" | jq -r '.task.creator_name')
task_project_name=$(echo "$body" | jq -r '.task.project_name')
task_task_type_name=$(echo "$body" | jq -r '.task.task_type_name')
task_scheduled_start_date=$(echo "$body" | jq -r '.task.scheduled_start_date')
task_scheduled_end_date=$(echo "$body" | jq -r '.task.scheduled_end_date')
task_scheduled_completion_date=$(echo "$body" | jq -r '.task.scheduled_completion_date')
task_completion_date=$(echo "$body" | jq -r '.task.completion_date')
task_progress=$(echo "$body" | jq -r '.task.progress')
task_completed=$(echo "$body" | jq -r '.task.completed')
task_status=$(echo "$body" | jq -r '.task.status')
task_status_last_changed=$(echo "$body" | jq -r '.task.status_last_changed')
task_participants=$(echo "$body" | jq -c '.task.participants')
if [ "$task_project_id" = "$project_id_send" ] && [ "$task_name" = "$name_send" ] && [ "$task_description" = "$description_send" ] && [ "$task_remark" = "$remark_send" ]; then
  result_json=$(jq -n \
    --arg id "$task_id" \
    --arg another_task_id "$another_task_id" \
    --arg project_id "$task_project_id" \
    --arg task_type_id "$task_task_type_id" \
    --arg name "$task_name" \
    --arg description "$task_description" \
    --arg concurrency_stamp "$task_concurrency_stamp" \
    --arg created_at "$task_created_at" \
    --arg remark "$task_remark" \
    --arg creator_id "$task_creator_id" \
    --arg creator_name "$task_creator_name" \
    --arg project_name "$task_project_name" \
    --arg task_type_name "$task_task_type_name" \
    --arg scheduled_start_date "$task_scheduled_start_date" \
    --arg scheduled_end_date "$task_scheduled_end_date" \
    --arg scheduled_completion_date "$task_scheduled_completion_date" \
    --arg completion_date "$task_completion_date" \
    --arg progress "$task_progress" \
    --arg completed "$task_completed" \
    --arg status_val "$task_status" \
    --arg status_last_changed "$task_status_last_changed" \
    --arg participants "$task_participants" \
    --arg status "success" \
    '{status: $status, id: $id, another_task_id: $another_task_id, project_id: $project_id, task_type_id: $task_type_id, name: $name, description: $description, concurrency_stamp: $concurrency_stamp, created_at: $created_at, remark: $remark, creator_id: $creator_id, creator_name: $creator_name, project_name: $project_name, task_type_name: $task_type_name, scheduled_start_date: $scheduled_start_date, scheduled_end_date: $scheduled_end_date, scheduled_completion_date: $scheduled_completion_date, completion_date: $completion_date, progress: $progress, completed: $completed, status_val: $status_val, status_last_changed: $status_last_changed, participants: $participants}')
  echo "$result_json"
else
  result_json=$(jq -n \
    --arg status "fail" \
    --arg msg "$(echo "$body" | jq -c .)" \
    '{status: $status, msg: $msg, id: '123'}')
  echo "$result_json"
fi