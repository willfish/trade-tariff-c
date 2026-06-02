#ifndef TRADE_TARIFF_C_MEURSING_HEADING_TEXT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEURSING_HEADING_TEXT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *meursing_table_plan_id;
    char *meursing_heading_number;
    char *row_column_code;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeursingHeadingTextOplogModel;

typedef struct {
    MeursingHeadingTextOplogModel *items;
    size_t len;
    size_t cap;
} MeursingHeadingTextOplogModelList;

#endif
