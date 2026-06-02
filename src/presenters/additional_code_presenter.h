#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_PRESENTER_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_PRESENTER_H

#include "models/additional_code_model.h"

const char *additional_code_presenter_resource_type(void);
const char *additional_code_presenter_id(const AdditionalCodeModel *model);
char *additional_code_presenter_code(const AdditionalCodeModel *model);
char *additional_code_presenter_description(const AdditionalCodeModel *model, int uk);
int additional_code_presenter_self_test(void);

#endif
