#ifndef TRADE_TARIFF_C_MEURSING_TABLE_PLAN_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEURSING_TABLE_PLAN_OPLOG_PRESENTER_H

#include "models/oplog/meursing_table_plan_model.h"

const char *meursing_table_plan_oplog_presenter_resource_type(void);
const char *meursing_table_plan_oplog_presenter_id(const MeursingTablePlanOplogModel *model);
int meursing_table_plan_oplog_presenter_self_test(void);

#endif
