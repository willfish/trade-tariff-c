#ifndef TRADE_TARIFF_C_MEASURE_TYPE_MODEL_H
#define TRADE_TARIFF_C_MEASURE_TYPE_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_type_id;
    char *description;
    char *measure_type_series_id;
    char *measure_type_series_description;
    char *measure_component_applicable_code;
    char *order_number_capture_code;
    char *trade_movement_code;
    char *validity_start_date;
    char *validity_end_date;
} MeasureTypeModel;

typedef struct {
    MeasureTypeModel *items;
    size_t len;
    size_t cap;
} MeasureTypeModelList;

void measure_type_model_free(MeasureTypeModel *model);
void measure_type_model_list_free(MeasureTypeModelList *list);
int measure_type_model_list_append(MeasureTypeModelList *list, MeasureTypeModel model);
int measure_type_model_self_test(void);

#endif
