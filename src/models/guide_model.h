#ifndef TRADE_TARIFF_C_GUIDE_MODEL_H
#define TRADE_TARIFF_C_GUIDE_MODEL_H

#include <stddef.h>

typedef struct {
    char *id;
    char *title;
    char *url;
} GuideModel;

typedef struct {
    GuideModel *items;
    size_t len;
    size_t cap;
} GuideModelList;

void guide_model_free(GuideModel *model);
void guide_model_list_free(GuideModelList *list);
int guide_model_list_append(GuideModelList *list, GuideModel model);
int guide_model_self_test(void);

#endif
