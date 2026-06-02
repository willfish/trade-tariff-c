#ifndef TRADE_TARIFF_C_CERTIFICATE_PRESENTER_H
#define TRADE_TARIFF_C_CERTIFICATE_PRESENTER_H

#include "models/certificate_model.h"

const char *certificate_presenter_resource_type(void);
const char *certificate_presenter_id(const CertificateModel *model);
int certificate_presenter_self_test(void);

#endif
