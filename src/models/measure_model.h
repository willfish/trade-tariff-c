#ifndef TRADE_TARIFF_C_MEASURE_MODEL_H
#define TRADE_TARIFF_C_MEASURE_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *measure_type_id;
    char *measure_type_description;
    char *geographical_area_sid;
    char *geographical_area_id;
    char *additional_code_sid;
    char *additional_code_type_id;
    char *additional_code_id;
    char *ordernumber;
    char *reduction_indicator;
    char *national;
    char *trade_movement_code;
    char *measure_type_series_id;
    char *measure_generating_regulation_role;
    char *measure_generating_regulation_id;
    char *validity_start_date;
    char *validity_end_date;
    char *effective_start_date;
    char *effective_end_date;
} MeasureModel;

typedef struct {
    MeasureModel *items;
    size_t len;
    size_t cap;
} MeasureModelList;

void measure_model_free(MeasureModel *model);
void measure_model_list_free(MeasureModelList *list);
int measure_model_list_append(MeasureModelList *list, MeasureModel model);
int measure_model_import(const MeasureModel *model);
int measure_model_export(const MeasureModel *model);
int measure_model_self_test(void);

#endif
