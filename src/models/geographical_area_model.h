#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_MODEL_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_MODEL_H

#include <stddef.h>

typedef struct {
    char *geographical_area_sid;
    char *geographical_area_id;
    char *description;
    char *parent_geographical_area_sid;
} GeographicalAreaModel;

typedef struct {
    GeographicalAreaModel *items;
    size_t len;
    size_t cap;
} GeographicalAreaModelList;

void geographical_area_model_free(GeographicalAreaModel *model);
void geographical_area_model_list_free(GeographicalAreaModelList *list);
int geographical_area_model_list_append(GeographicalAreaModelList *list,
                                        GeographicalAreaModel model);
int geographical_area_model_self_test(void);

#endif
