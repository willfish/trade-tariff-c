#ifndef TRADE_TARIFF_C_MEASURE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_OPLOG_PRESENTER_H

#include "models/oplog/measure_model.h"

const char *measure_oplog_presenter_resource_type(void);
const char *measure_oplog_presenter_id(const MeasureOplogModel *model);
int measure_oplog_presenter_self_test(void);

#endif
