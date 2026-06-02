#ifndef TRADE_TARIFF_C_COMMODITY_REQUEST_CONTEXT_H
#define TRADE_TARIFF_C_COMMODITY_REQUEST_CONTEXT_H

typedef struct {
    const char *schema;
    const char *actual_date;
    const char *commodity_id;
    const char *meursing_additional_code_id;
} CommodityRequestContext;

#endif
