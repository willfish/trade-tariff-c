#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_JSONAPI_RENDERER_H

#include "models/geographical_area_model.h"
#include "models/measure_excluded_geographical_area_model.h"

#include <yyjson.h>

yyjson_mut_val *geographical_area_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const char *geographical_area_id);
yyjson_mut_val *geographical_area_jsonapi_excluded_relationship_data_for_measure(
    yyjson_mut_doc *doc,
    const MeasureExcludedGeographicalAreaModelList *excluded,
    const char *measure_sid);
yyjson_mut_val *geographical_area_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const GeographicalAreaModel *area,
                                                   const GeographicalAreaModelList *all_areas);
int geographical_area_jsonapi_renderer_self_test(void);

#endif
