#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *geographical_area_description_period_sid;
    char *language_id;
    char *geographical_area_sid;
    char *geographical_area_id;
    char *description;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} GeographicalAreaDescriptionOplogModel;

typedef struct {
    GeographicalAreaDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} GeographicalAreaDescriptionOplogModelList;

#endif
