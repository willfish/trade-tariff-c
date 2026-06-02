#ifndef TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_MODEL_H
#define TRADE_TARIFF_C_QUOTA_ORDER_NUMBER_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_order_number_sid;
    char *quota_order_number_id;
    char *validity_start_date;
    char *validity_end_date;
} QuotaOrderNumberModel;

typedef struct {
    QuotaOrderNumberModel *items;
    size_t len;
    size_t cap;
} QuotaOrderNumberModelList;

void quota_order_number_model_free(QuotaOrderNumberModel *model);
void quota_order_number_model_list_free(QuotaOrderNumberModelList *list);
int quota_order_number_model_list_append(QuotaOrderNumberModelList *list,
                                         QuotaOrderNumberModel model);
int quota_order_number_model_self_test(void);

#endif
