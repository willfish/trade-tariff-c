#ifndef TRADE_TARIFF_C_REGULATION_REPLACEMENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_REGULATION_REPLACEMENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *geographical_area_id;
    char *chapter_heading;
    char *replacing_regulation_role;
    char *replacing_regulation_id;
    char *replaced_regulation_role;
    char *replaced_regulation_id;
    char *measure_type_id;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} RegulationReplacementOplogModel;

typedef struct {
    RegulationReplacementOplogModel *items;
    size_t len;
    size_t cap;
} RegulationReplacementOplogModelList;

#endif
