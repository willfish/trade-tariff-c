#ifndef TRADE_TARIFF_C_QUOTA_ASSOCIATION_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_QUOTA_ASSOCIATION_OPLOG_PRESENTER_H

#include "models/oplog/quota_association_model.h"

const char *quota_association_oplog_presenter_resource_type(void);
const char *quota_association_oplog_presenter_id(const QuotaAssociationOplogModel *model);
int quota_association_oplog_presenter_self_test(void);

#endif
