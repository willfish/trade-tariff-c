#ifndef TRADE_TARIFF_C_MEASURE_EXCLUDED_GEOGRAPHICAL_AREA_MODEL_H
#define TRADE_TARIFF_C_MEASURE_EXCLUDED_GEOGRAPHICAL_AREA_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *geographical_area_sid;
    char *geographical_area_id;
} MeasureExcludedGeographicalAreaModel;

typedef struct {
    MeasureExcludedGeographicalAreaModel *items;
    size_t len;
    size_t cap;
} MeasureExcludedGeographicalAreaModelList;

void measure_excluded_geographical_area_model_free(MeasureExcludedGeographicalAreaModel *model);
void measure_excluded_geographical_area_model_list_free(MeasureExcludedGeographicalAreaModelList *list);
int measure_excluded_geographical_area_model_list_append(MeasureExcludedGeographicalAreaModelList *list,
                                                         MeasureExcludedGeographicalAreaModel model);
int measure_excluded_geographical_area_model_self_test(void);

#endif
