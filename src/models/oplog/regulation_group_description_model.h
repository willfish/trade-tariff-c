#ifndef TRADE_TARIFF_C_REGULATION_GROUP_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_REGULATION_GROUP_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *regulation_group_id;
    char *language_id;
    char *description;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} RegulationGroupDescriptionOplogModel;

typedef struct {
    RegulationGroupDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} RegulationGroupDescriptionOplogModelList;

#endif
