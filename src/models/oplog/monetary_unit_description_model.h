#ifndef TRADE_TARIFF_C_MONETARY_UNIT_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MONETARY_UNIT_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *monetary_unit_code;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MonetaryUnitDescriptionOplogModel;

typedef struct {
    MonetaryUnitDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} MonetaryUnitDescriptionOplogModelList;

#endif
