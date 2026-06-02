#ifndef TRADE_TARIFF_C_MEASURE_TYPE_SERIES_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_TYPE_SERIES_OPLOG_PRESENTER_H

#include "models/oplog/measure_type_series_model.h"

const char *measure_type_series_oplog_presenter_resource_type(void);
const char *measure_type_series_oplog_presenter_id(const MeasureTypeSeriesOplogModel *model);
int measure_type_series_oplog_presenter_self_test(void);

#endif
