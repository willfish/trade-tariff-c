#ifndef TRADE_TARIFF_C_MEASURE_LEGAL_ACT_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_LEGAL_ACT_PRESENTER_H

#include "models/measure_legal_act_model.h"

const char *measure_legal_act_presenter_resource_type(void);
const char *measure_legal_act_presenter_suspension_resource_type(void);
const char *measure_legal_act_presenter_id(const MeasureLegalActModel *model);
int measure_legal_act_presenter_is_suspension(const MeasureLegalActModel *model);
int measure_legal_act_presenter_reduced_info(const MeasureLegalActModel *model);
char *measure_legal_act_presenter_regulation_code(const MeasureLegalActModel *model);
char *measure_legal_act_presenter_regulation_url(const MeasureLegalActModel *model);
char *measure_legal_act_presenter_suspension_regulation_code(const MeasureLegalActModel *model);
char *measure_legal_act_presenter_suspension_regulation_url(const MeasureLegalActModel *model);
const char *measure_legal_act_presenter_description(const MeasureLegalActModel *model);
const MeasureLegalActModel *measure_legal_act_presenter_suspension_for_measure(
    const MeasureLegalActModelList *legal_acts,
    const char *measure_sid);
int measure_legal_act_presenter_self_test(void);

#endif
