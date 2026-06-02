#ifndef TRADE_TARIFF_C_SUBHEADING_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_SUBHEADING_OPLOG_PRESENTER_H

#include "models/oplog/subheading_model.h"

const char *subheading_oplog_presenter_resource_type(void);
const char *subheading_oplog_presenter_id(const SubheadingOplogModel *model);
int subheading_oplog_presenter_self_test(void);

#endif
