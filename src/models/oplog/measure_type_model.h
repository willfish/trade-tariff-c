#ifndef TRADE_TARIFF_C_MEASURE_TYPE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_TYPE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_type_id;
    char *validity_start_date;
    char *validity_end_date;
    char *trade_movement_code;
    char *priority_code;
    char *measure_component_applicable_code;
    char *origin_dest_code;
    char *order_number_capture_code;
    char *measure_explosion_level;
    char *measure_type_series_id;
    char *national;
    char *measure_type_acronym;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureTypeOplogModel;

typedef struct {
    MeasureTypeOplogModel *items;
    size_t len;
    size_t cap;
} MeasureTypeOplogModelList;

#endif
