#include "get-project-complete-date.h"

const std::string getProjectCompleteDateSql = R"SQL(
    SELECT estimation_date, complete_date
    FROM project_estimation_histories
    WHERE project_id = $1::uuid
    ORDER BY complete_date ASC
)SQL";