#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_OPLOG_PRESENTER_H

#include "models/oplog/geographical_area_model.h"

const char *geographical_area_oplog_presenter_resource_type(void);
const char *geographical_area_oplog_presenter_id(const GeographicalAreaOplogModel *model);
int geographical_area_oplog_presenter_self_test(void);

#endif
