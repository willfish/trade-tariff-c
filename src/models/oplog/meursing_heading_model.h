#ifndef TRADE_TARIFF_C_MEURSING_HEADING_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEURSING_HEADING_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *meursing_table_plan_id;
    char *meursing_heading_number;
    char *row_column_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeursingHeadingOplogModel;

typedef struct {
    MeursingHeadingOplogModel *items;
    size_t len;
    size_t cap;
} MeursingHeadingOplogModelList;

#endif
