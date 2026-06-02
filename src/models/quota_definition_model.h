#ifndef TRADE_TARIFF_C_QUOTA_DEFINITION_MODEL_H
#define TRADE_TARIFF_C_QUOTA_DEFINITION_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *quota_order_number_id;
    char *quota_order_number_sid;
    char *initial_volume;
    char *balance;
    char *description;
    char *measurement_unit_code;
    char *monetary_unit_code;
    char *measurement_unit_qualifier_code;
    char *last_allocation_date;
    char *suspension_period_start_date;
    char *suspension_period_end_date;
    char *blocking_period_start_date;
    char *blocking_period_end_date;
    char *validity_start_date;
    char *validity_end_date;
    char *measurement_unit_description;
    char *measurement_unit_abbreviation;
    char *critical_state;
    char *latest_critical_state;
    char *exhausted_event_sid;
} QuotaDefinitionModel;

typedef struct {
    QuotaDefinitionModel *items;
    size_t len;
    size_t cap;
} QuotaDefinitionModelList;

void quota_definition_model_free(QuotaDefinitionModel *model);
void quota_definition_model_list_free(QuotaDefinitionModelList *list);
int quota_definition_model_list_append(QuotaDefinitionModelList *list,
                                       QuotaDefinitionModel model);
int quota_definition_model_self_test(void);

#endif
