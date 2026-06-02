#ifndef TRADE_TARIFF_C_REGULATION_GROUP_OPLOG_MODEL_H
#define TRADE_TARIFF_C_REGULATION_GROUP_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *regulation_group_id;
    char *validity_start_date;
    char *validity_end_date;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} RegulationGroupOplogModel;

typedef struct {
    RegulationGroupOplogModel *items;
    size_t len;
    size_t cap;
} RegulationGroupOplogModelList;

#endif
