#ifndef TRADE_TARIFF_C_CERTIFICATE_TYPE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_CERTIFICATE_TYPE_OPLOG_PRESENTER_H

#include "models/oplog/certificate_type_model.h"

const char *certificate_type_oplog_presenter_resource_type(void);
const char *certificate_type_oplog_presenter_id(const CertificateTypeOplogModel *model);
int certificate_type_oplog_presenter_self_test(void);

#endif
