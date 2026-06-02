#ifndef TRADE_TARIFF_C_MEASURE_CONDITION_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_CONDITION_PRESENTER_H

#include "models/appendix_5a_model.h"
#include "models/measure_condition_component_model.h"
#include "models/measure_condition_model.h"

const char *measure_condition_presenter_resource_type(void);
const char *measure_condition_presenter_id(const MeasureConditionModel *model);
char *measure_condition_presenter_document_code(const MeasureConditionModel *model);
char *measure_condition_presenter_condition(const MeasureConditionModel *model);
char *measure_condition_presenter_requirement(const MeasureConditionModel *model);
char *measure_condition_presenter_duty_expression(const MeasureConditionModel *model,
                                                  const MeasureConditionComponentModelList *components);
const char *measure_condition_presenter_monetary_unit_abbreviation(const MeasureConditionModel *model);
const char *measure_condition_presenter_guidance_cds(const MeasureConditionModel *model,
                                                     const Appendix5aModelList *appendix_5as);
const char *measure_condition_presenter_measure_condition_class(const MeasureConditionModel *model);
const char *measure_condition_presenter_requirement_operator(const MeasureConditionModel *model);
const char *measure_condition_presenter_threshold_unit_type(const MeasureConditionModel *model);
int measure_condition_presenter_self_test(void);

#endif
