#ifndef TRADE_TARIFF_C_DUTY_EXPRESSION_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_DUTY_EXPRESSION_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *duty_expression_id;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} DutyExpressionDescriptionOplogModel;

typedef struct {
    DutyExpressionDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} DutyExpressionDescriptionOplogModelList;

#endif
