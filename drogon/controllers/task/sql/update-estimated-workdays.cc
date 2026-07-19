const char* updateTaskEstimatedWorkdaysSql = R"SQL(
    UPDATE tasks SET estimated_workdays = $1::int WHERE id = $2::uuid
)SQL";