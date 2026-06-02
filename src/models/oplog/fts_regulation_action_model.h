#ifndef TRADE_TARIFF_C_FTS_REGULATION_ACTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FTS_REGULATION_ACTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *fts_regulation_role;
    char *fts_regulation_id;
    char *stopped_regulation_role;
    char *stopped_regulation_id;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FtsRegulationActionOplogModel;

typedef struct {
    FtsRegulationActionOplogModel *items;
    size_t len;
    size_t cap;
} FtsRegulationActionOplogModelList;

#endif
