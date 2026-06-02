#ifndef TRADE_TARIFF_C_MONETARY_UNIT_MODEL_H
#define TRADE_TARIFF_C_MONETARY_UNIT_MODEL_H

#include <stddef.h>

typedef struct {
    char *monetary_unit_code;
    char *description;
} MonetaryUnitModel;

typedef struct {
    MonetaryUnitModel *items;
    size_t len;
    size_t cap;
} MonetaryUnitModelList;

#endif
