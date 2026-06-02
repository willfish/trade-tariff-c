#ifndef TRADE_TARIFF_C_CERTIFICATE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_CERTIFICATE_OPLOG_PRESENTER_H

#include "models/oplog/certificate_model.h"

const char *certificate_oplog_presenter_resource_type(void);
const char *certificate_oplog_presenter_id(const CertificateOplogModel *model);
int certificate_oplog_presenter_self_test(void);

#endif
