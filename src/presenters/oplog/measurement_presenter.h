#ifndef TRADE_TARIFF_C_MEASUREMENT_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASUREMENT_OPLOG_PRESENTER_H

#include "models/oplog/measurement_model.h"

const char *measurement_oplog_presenter_resource_type(void);
const char *measurement_oplog_presenter_id(const MeasurementOplogModel *model);
int measurement_oplog_presenter_self_test(void);

#endif
