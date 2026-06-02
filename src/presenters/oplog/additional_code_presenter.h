#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_OPLOG_PRESENTER_H

#include "models/oplog/additional_code_model.h"

const char *additional_code_oplog_presenter_resource_type(void);
const char *additional_code_oplog_presenter_id(const AdditionalCodeOplogModel *model);
int additional_code_oplog_presenter_self_test(void);

#endif
