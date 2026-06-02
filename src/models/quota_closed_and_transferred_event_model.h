#ifndef TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_MODEL_H
#define TRADE_TARIFF_C_QUOTA_CLOSED_AND_TRANSFERRED_EVENT_MODEL_H

#include <stddef.h>

typedef struct {
    char *quota_definition_sid;
    char *occurrence_timestamp;
    char *target_quota_definition_sid;
    char *closing_date;
    char *transferred_amount;
    char *quota_definition_validity_start_date;
    char *quota_definition_validity_end_date;
    char *quota_definition_measurement_unit_description;
    char *quota_definition_measurement_unit_abbreviation;
    char *target_quota_definition_validity_start_date;
    char *target_quota_definition_validity_end_date;
    char *target_quota_definition_measurement_unit_description;
    char *target_quota_definition_measurement_unit_abbreviation;
} QuotaClosedAndTransferredEventModel;

typedef struct {
    QuotaClosedAndTransferredEventModel *items;
    size_t len;
    size_t cap;
} QuotaClosedAndTransferredEventModelList;

void quota_closed_and_transferred_event_model_free(QuotaClosedAndTransferredEventModel *model);
void quota_closed_and_transferred_event_model_list_free(QuotaClosedAndTransferredEventModelList *list);
int quota_closed_and_transferred_event_model_list_append(QuotaClosedAndTransferredEventModelList *list,
                                                         QuotaClosedAndTransferredEventModel model);
int quota_closed_and_transferred_event_model_self_test(void);

#endif
