#ifndef TRADE_TARIFF_C_REGULATION_GROUP_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_REGULATION_GROUP_OPLOG_PRESENTER_H

#include "models/oplog/regulation_group_model.h"

const char *regulation_group_oplog_presenter_resource_type(void);
const char *regulation_group_oplog_presenter_id(const RegulationGroupOplogModel *model);
int regulation_group_oplog_presenter_self_test(void);

#endif
