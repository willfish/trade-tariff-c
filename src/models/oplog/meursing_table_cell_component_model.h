#ifndef TRADE_TARIFF_C_MEURSING_TABLE_CELL_COMPONENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEURSING_TABLE_CELL_COMPONENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *meursing_additional_code_sid;
    char *meursing_table_plan_id;
    char *heading_number;
    char *row_column_code;
    char *subheading_sequence_number;
    char *validity_start_date;
    char *validity_end_date;
    char *additional_code;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeursingTableCellComponentOplogModel;

typedef struct {
    MeursingTableCellComponentOplogModel *items;
    size_t len;
    size_t cap;
} MeursingTableCellComponentOplogModelList;

#endif
