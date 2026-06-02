#ifndef TRADE_TARIFF_C_MONETARY_UNIT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MONETARY_UNIT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *monetary_unit_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MonetaryUnitOplogModel;

typedef struct {
    MonetaryUnitOplogModel *items;
    size_t len;
    size_t cap;
} MonetaryUnitOplogModelList;

#endif
