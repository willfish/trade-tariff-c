#ifndef TRADE_TARIFF_C_MEURSING_SUBHEADING_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_MEURSING_SUBHEADING_OPLOG_PRESENTER_H

#include "models/oplog/meursing_subheading_model.h"

const char *meursing_subheading_oplog_presenter_resource_type(void);
const char *meursing_subheading_oplog_presenter_id(const MeursingSubheadingOplogModel *model);
int meursing_subheading_oplog_presenter_self_test(void);

#endif
