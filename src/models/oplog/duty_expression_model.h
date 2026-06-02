#ifndef TRADE_TARIFF_C_DUTY_EXPRESSION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_DUTY_EXPRESSION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *duty_expression_id;
    char *validity_start_date;
    char *validity_end_date;
    char *duty_amount_applicability_code;
    char *measurement_unit_applicability_code;
    char *monetary_unit_applicability_code;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} DutyExpressionOplogModel;

typedef struct {
    DutyExpressionOplogModel *items;
    size_t len;
    size_t cap;
} DutyExpressionOplogModelList;

#endif
