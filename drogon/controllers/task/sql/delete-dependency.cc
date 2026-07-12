#include "sql.h"

const char *deleteTaskDependencySql = R"SQL(
    DELETE FROM work_item_dependencies WHERE predecessor_id = $1 AND successor_id = $2
)SQL";