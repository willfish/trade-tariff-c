#ifndef TRADE_TARIFF_C_BASE_REGULATION_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_BASE_REGULATION_OPLOG_PRESENTER_H

#include "models/oplog/base_regulation_model.h"

const char *base_regulation_oplog_presenter_resource_type(void);
const char *base_regulation_oplog_presenter_id(const BaseRegulationOplogModel *model);
int base_regulation_oplog_presenter_self_test(void);

#endif
