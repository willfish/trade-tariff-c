#ifndef TRADE_TARIFF_C_MEURSING_TABLE_PLAN_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEURSING_TABLE_PLAN_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *meursing_table_plan_id;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeursingTablePlanOplogModel;

typedef struct {
    MeursingTablePlanOplogModel *items;
    size_t len;
    size_t cap;
} MeursingTablePlanOplogModelList;

#endif
