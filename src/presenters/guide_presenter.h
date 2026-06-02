#ifndef TRADE_TARIFF_C_GUIDE_PRESENTER_H
#define TRADE_TARIFF_C_GUIDE_PRESENTER_H

#include "models/guide_model.h"

const char *guide_presenter_resource_type(void);
const char *guide_presenter_id(const GuideModel *guide);
int guide_presenter_self_test(void);

#endif
