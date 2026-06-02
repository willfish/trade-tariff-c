#ifndef TRADE_TARIFF_C_FOOTNOTE_TYPE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_FOOTNOTE_TYPE_OPLOG_PRESENTER_H

#include "models/oplog/footnote_type_model.h"

const char *footnote_type_oplog_presenter_resource_type(void);
const char *footnote_type_oplog_presenter_id(const FootnoteTypeOplogModel *model);
int footnote_type_oplog_presenter_self_test(void);

#endif
