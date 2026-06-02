#ifndef TRADE_TARIFF_C_PREFERENCE_CODE_PRESENTER_H
#define TRADE_TARIFF_C_PREFERENCE_CODE_PRESENTER_H

#include "models/measure_condition_model.h"
#include "models/measure_model.h"

typedef struct {
    int authorised_use_provisions_submission;
    int special_nature;
} PreferenceCodeDeclarable;

typedef struct {
    const char *measure_type_id;
    const char *geographical_area_id;
    int import;
    int authorised_use;
    int gsp_or_dcts;
} PreferenceCodeMeasure;

const char *preference_code_presenter_determine_code(const PreferenceCodeDeclarable *declarable,
                                                     const PreferenceCodeMeasure *measure);
PreferenceCodeDeclarable preference_code_presenter_declarable_from_measures(const MeasureModelList *measures,
                                                                            const MeasureConditionModelList *conditions);
const char *preference_code_presenter_code_for_measure(const PreferenceCodeDeclarable *declarable,
                                                       const MeasureModel *measure,
                                                       const MeasureConditionModelList *conditions);
const char *preference_code_presenter_resource_type(void);
const char *preference_code_presenter_id(const char *code);
const char *preference_code_presenter_description(const char *code);
int preference_code_presenter_self_test(void);

#endif
