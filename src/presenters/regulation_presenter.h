#ifndef TRADE_TARIFF_C_REGULATION_PRESENTER_H
#define TRADE_TARIFF_C_REGULATION_PRESENTER_H

#include "models/regulation_model.h"

const char *regulation_presenter_resource_type(void);
const char *regulation_presenter_id(const RegulationModel *model);
int regulation_presenter_self_test(void);

#endif
