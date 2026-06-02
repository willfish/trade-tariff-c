#ifndef TRADE_TARIFF_C_QUOTA_DEFINITION_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_QUOTA_DEFINITION_OPLOG_PRESENTER_H

#include "models/oplog/quota_definition_model.h"

const char *quota_definition_oplog_presenter_resource_type(void);
const char *quota_definition_oplog_presenter_id(const QuotaDefinitionOplogModel *model);
int quota_definition_oplog_presenter_self_test(void);

#endif
