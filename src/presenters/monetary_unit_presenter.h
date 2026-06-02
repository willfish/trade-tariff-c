#ifndef TRADE_TARIFF_C_MONETARY_UNIT_PRESENTER_H
#define TRADE_TARIFF_C_MONETARY_UNIT_PRESENTER_H

#include "models/monetary_unit_model.h"

const char *monetary_unit_presenter_resource_type(void);
const char *monetary_unit_presenter_id(const MonetaryUnitModel *model);
const char *monetary_unit_presenter_abbreviation_for_code(const char *monetary_unit_code);
int monetary_unit_presenter_self_test(void);

#endif
