#ifndef TRADE_TARIFF_C_MEASURE_PRESENTER_H
#define TRADE_TARIFF_C_MEASURE_PRESENTER_H

#include "models/measure_model.h"
#include "models/measure_component_model.h"

const char *measure_presenter_resource_type(void);
const char *measure_presenter_id(const MeasureModel *model);
long long measure_presenter_numeric_id(const MeasureModel *model);
const char *measure_presenter_origin(const MeasureModel *model);
int measure_presenter_excise(const MeasureModel *model);
int measure_presenter_vat(const MeasureModel *model);
int measure_presenter_meursing(const MeasureModel *model,
                               const MeasureComponentModelList *components);
int measure_presenter_self_test(void);

#endif
