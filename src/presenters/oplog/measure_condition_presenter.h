#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_OPLOG_PRESENTER_H

#include "models/oplog/measure_condition_model.h"

const char *measure_condition_oplog_presenter_resource_type(void);
const char *measure_condition_oplog_presenter_id(const MeasureConditionOplogModel *model);
int measure_condition_oplog_presenter_self_test(void);

#endif
