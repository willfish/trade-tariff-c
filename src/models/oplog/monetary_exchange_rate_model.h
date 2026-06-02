#ifndef TRADE_TARIFF_C_MONETARY_EXCHANGE_RATE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MONETARY_EXCHANGE_RATE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *monetary_exchange_period_sid;
    char *child_monetary_unit_code;
    char *exchange_rate;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MonetaryExchangeRateOplogModel;

typedef struct {
    MonetaryExchangeRateOplogModel *items;
    size_t len;
    size_t cap;
} MonetaryExchangeRateOplogModelList;

#endif
