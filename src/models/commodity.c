#include "models/commodity.h"

#include "common.h"

#include <ctype.h>
#include <stddef.h>
#include <string.h>

CommodityRequest commodity_request_new(const char *service, const char *commodity_id)
{
    return (CommodityRequest){
        .service = service,
        .commodity_id = commodity_id,
        .actual_date = NULL,
        .meursing_additional_code_id = NULL,
        .include_plan = commodity_include_plan_default(),
    };
}

static int valid_service(const char *service)
{
    return service != NULL && (strcmp(service, "uk") == 0 || strcmp(service, "xi") == 0);
}

static int valid_commodity_id(const char *commodity_id)
{
    if (commodity_id == NULL || strlen(commodity_id) != MAX_COMMODITY_ID_LEN) {
        return 0;
    }

    for (const char *p = commodity_id; *p; p++) {
        if (!isdigit((unsigned char)*p)) {
            return 0;
        }
    }

    return 1;
}

static int valid_actual_date(const char *actual_date)
{
    if (actual_date == NULL || *actual_date == '\0') {
        return 1;
    }

    if (strlen(actual_date) != 10) {
        return 0;
    }

    for (size_t i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (actual_date[i] != '-') {
                return 0;
            }
        } else if (!isdigit((unsigned char)actual_date[i])) {
            return 0;
        }
    }

    return 1;
}

CommodityRequestValidation commodity_request_validate(const CommodityRequest *request)
{
    if (request == NULL || !valid_service(request->service)) {
        return COMMODITY_REQUEST_BAD_SERVICE;
    }

    if (!valid_commodity_id(request->commodity_id)) {
        return COMMODITY_REQUEST_BAD_COMMODITY_ID;
    }

    if (!valid_actual_date(request->actual_date)) {
        return COMMODITY_REQUEST_BAD_ACTUAL_DATE;
    }

    return COMMODITY_REQUEST_VALID;
}

const char *commodity_request_validation_message(CommodityRequestValidation validation)
{
    switch (validation) {
    case COMMODITY_REQUEST_BAD_SERVICE:
        return "SERVICE must be uk or xi";
    case COMMODITY_REQUEST_BAD_COMMODITY_ID:
        return "Commodity id must be 10 digits";
    case COMMODITY_REQUEST_BAD_ACTUAL_DATE:
        return "ACTUAL_DATE must use YYYY-MM-DD";
    case COMMODITY_REQUEST_VALID:
    default:
        return "";
    }
}

int commodity_request_self_test(void)
{
    CommodityRequest uk = commodity_request_new("uk", "0101210000");
    CommodityRequest xi = commodity_request_new("xi", "0101210000");
    CommodityRequest bad_service = commodity_request_new("public", "0101210000");
    CommodityRequest short_id = commodity_request_new("uk", "01012100");
    CommodityRequest bad_id = commodity_request_new("uk", "010121000x");
    CommodityRequest dated = commodity_request_new("uk", "0101210000");
    CommodityRequest bad_date = commodity_request_new("uk", "0101210000");
    dated.actual_date = "2026-05-22";
    bad_date.actual_date = "2026-5-22";

    return commodity_request_validate(&dated) == COMMODITY_REQUEST_VALID &&
           commodity_include_plan_enabled(&uk.include_plan, COMMODITY_INCLUDE_IMPORT_MEASURES) &&
           commodity_request_validate(&xi) == COMMODITY_REQUEST_VALID &&
           commodity_request_validate(&bad_service) == COMMODITY_REQUEST_BAD_SERVICE &&
           commodity_request_validate(&short_id) == COMMODITY_REQUEST_BAD_COMMODITY_ID &&
           commodity_request_validate(&bad_id) == COMMODITY_REQUEST_BAD_COMMODITY_ID &&
           commodity_request_validate(&bad_date) == COMMODITY_REQUEST_BAD_ACTUAL_DATE;
}
