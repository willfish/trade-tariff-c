#ifndef TRADE_TARIFF_C_GEOGRAPHICAL_AREA_OPLOG_MODEL_H
#define TRADE_TARIFF_C_GEOGRAPHICAL_AREA_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *geographical_area_sid;
    char *parent_geographical_area_group_sid;
    char *validity_start_date;
    char *validity_end_date;
    char *geographical_code;
    char *geographical_area_id;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
    char *hjid;
} GeographicalAreaOplogModel;

typedef struct {
    GeographicalAreaOplogModel *items;
    size_t len;
    size_t cap;
} GeographicalAreaOplogModelList;

#endif
