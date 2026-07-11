#include "get-milestone-complete-date.h"

const std::string getMilestoneCompleteDateSql = R"SQL(
    SELECT estimation_date, complete_date
    FROM milestone_estimation_histories
    WHERE milestone_id = $1::uuid
    ORDER BY complete_date ASC
)SQL";