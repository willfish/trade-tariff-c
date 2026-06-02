#ifndef TRADE_TARIFF_C_MEASUREMENT_UNIT_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASUREMENT_UNIT_OPLOG_PRESENTER_H

#include "models/oplog/measurement_unit_model.h"

const char *measurement_unit_oplog_presenter_resource_type(void);
const char *measurement_unit_oplog_presenter_id(const MeasurementUnitOplogModel *model);
int measurement_unit_oplog_presenter_self_test(void);

#endif
