#ifndef TRADE_TARIFF_C_COMMODITY_MODEL_H
#define TRADE_TARIFF_C_COMMODITY_MODEL_H

#include "request/commodity_include_plan.h"

typedef struct {
    const char *service;
    const char *commodity_id;
    const char *actual_date;
    const char *meursing_additional_code_id;
    CommodityIncludePlan include_plan;
} CommodityRequest;

typedef enum {
    COMMODITY_REQUEST_VALID = 0,
    COMMODITY_REQUEST_BAD_SERVICE,
    COMMODITY_REQUEST_BAD_COMMODITY_ID,
    COMMODITY_REQUEST_BAD_ACTUAL_DATE
} CommodityRequestValidation;

CommodityRequest commodity_request_new(const char *service, const char *commodity_id);
CommodityRequestValidation commodity_request_validate(const CommodityRequest *request);
const char *commodity_request_validation_message(CommodityRequestValidation validation);
int commodity_request_self_test(void);

#endif
