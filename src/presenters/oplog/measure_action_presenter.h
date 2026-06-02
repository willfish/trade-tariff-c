#ifndef TRADE_TARIFF_C_MEASURE_ACTION_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_ACTION_OPLOG_PRESENTER_H

#include "models/oplog/measure_action_model.h"

const char *measure_action_oplog_presenter_resource_type(void);
const char *measure_action_oplog_presenter_id(const MeasureActionOplogModel *model);
int measure_action_oplog_presenter_self_test(void);

#endif
