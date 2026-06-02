#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_PRESENTER_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_PRESENTER_H

#include "models/geographical_area_model.h"
#include "models/measure_excluded_geographical_area_model.h"

const char *geographical_area_presenter_resource_type(void);
const char *geographical_area_presenter_id(const GeographicalAreaModel *model);
const char *geographical_area_presenter_excluded_id(const MeasureExcludedGeographicalAreaModel *model);
int geographical_area_presenter_self_test(void);

#endif
