#ifndef TRADE_TARIFF_C_MEASURE_COMPONENT_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_COMPONENT_OPLOG_PRESENTER_H

#include "models/oplog/measure_component_model.h"

const char *measure_component_oplog_presenter_resource_type(void);
const char *measure_component_oplog_presenter_id(const MeasureComponentOplogModel *model);
int measure_component_oplog_presenter_self_test(void);

#endif
