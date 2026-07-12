const char* updateTaskStartDateSql = R"SQL(
    UPDATE tasks SET scheduled_start_date = $1::date WHERE id = $2::uuid
)SQL";