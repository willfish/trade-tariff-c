#ifndef TRADE_TARIFF_C_MONETARY_EXCHANGE_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MONETARY_EXCHANGE_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *monetary_exchange_period_sid;
    char *parent_monetary_unit_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MonetaryExchangePeriodOplogModel;

typedef struct {
    MonetaryExchangePeriodOplogModel *items;
    size_t len;
    size_t cap;
} MonetaryExchangePeriodOplogModelList;

#endif
