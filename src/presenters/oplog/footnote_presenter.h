#ifndef TRADE_TARIFF_C_FOOTNOTE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_FOOTNOTE_OPLOG_PRESENTER_H

#include "models/oplog/footnote_model.h"

const char *footnote_oplog_presenter_resource_type(void);
const char *footnote_oplog_presenter_id(const FootnoteOplogModel *model);
int footnote_oplog_presenter_self_test(void);

#endif
