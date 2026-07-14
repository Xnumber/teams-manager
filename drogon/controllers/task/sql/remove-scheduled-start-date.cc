#include "sql.h"
const char *removeScheduledStartDateSql = R"(
    UPDATE tasks
    SET scheduled_start_date = NULL
    WHERE id = $1;
)";