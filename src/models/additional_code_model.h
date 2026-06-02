#ifndef TRADE_TARIFF_C_ADDITIONAL_CODE_MODEL_H
#define TRADE_TARIFF_C_ADDITIONAL_CODE_MODEL_H

#include <stddef.h>

typedef struct {
    char *additional_code_sid;
    char *additional_code_type_id;
    char *additional_code;
    char *description;
} AdditionalCodeModel;

typedef struct {
    AdditionalCodeModel *items;
    size_t len;
    size_t cap;
} AdditionalCodeModelList;

void additional_code_model_free(AdditionalCodeModel *model);
void additional_code_model_list_free(AdditionalCodeModelList *list);
int additional_code_model_list_append(AdditionalCodeModelList *list,
                                      AdditionalCodeModel model);
int additional_code_model_self_test(void);

#endif
