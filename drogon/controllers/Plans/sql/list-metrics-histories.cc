#include "sql.h"

extern const char *listMetricsHistoriesSql = R"(
    SELECT * FROM plan_metrics_history WHERE plan_id = $1 ORDER BY date DESC;
)";