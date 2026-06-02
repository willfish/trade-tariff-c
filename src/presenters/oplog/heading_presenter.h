#ifndef TRADE_TARIFF_C_HEADING_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_HEADING_OPLOG_PRESENTER_H

#include "models/oplog/heading_model.h"

const char *heading_oplog_presenter_resource_type(void);
const char *heading_oplog_presenter_id(const HeadingOplogModel *model);
int heading_oplog_presenter_self_test(void);

#endif
