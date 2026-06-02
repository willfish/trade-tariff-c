#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_DESCRIPTION_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_DESCRIPTION_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *geographical_area_description_period_sid;
    char *geographical_area_sid;
    char *validity_start_date;
    char *geographical_area_id;
    char *created_at;
    char *validity_end_date;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GeographicalAreaDescriptionPeriodOplogModel;

typedef struct {
    GeographicalAreaDescriptionPeriodOplogModel *items;
    size_t len;
    size_t cap;
} GeographicalAreaDescriptionPeriodOplogModelList;

#endif
