#ifndef TRADE_TARIFF_C_MEASURE_LEGAL_ACT_MODEL_H
#define TRADE_TARIFF_C_MEASURE_LEGAL_ACT_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *measure_type_id;
    char *regulation_id;
    char *role;
    char *source;
    char *validity_start_date;
    char *validity_end_date;
    char *effective_start_date;
    char *effective_end_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *published_date;
    char *information_text;
} MeasureLegalActModel;

typedef struct {
    MeasureLegalActModel *items;
    size_t len;
    size_t cap;
} MeasureLegalActModelList;

void measure_legal_act_model_free(MeasureLegalActModel *model);
void measure_legal_act_model_list_free(MeasureLegalActModelList *list);
int measure_legal_act_model_list_append(MeasureLegalActModelList *list,
                                        MeasureLegalActModel model);
int measure_legal_act_model_self_test(void);

#endif
