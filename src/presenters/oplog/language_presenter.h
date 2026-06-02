#ifndef TRADE_TARIFF_C_LANGUAGE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_LANGUAGE_OPLOG_PRESENTER_H

#include "models/oplog/language_model.h"

const char *language_oplog_presenter_resource_type(void);
const char *language_oplog_presenter_id(const LanguageOplogModel *model);
int language_oplog_presenter_self_test(void);

#endif
