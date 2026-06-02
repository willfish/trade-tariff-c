#ifndef TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_MEURSING_HEADING_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_MEURSING_HEADING_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *meursing_table_plan_id;
    char *meursing_heading_number;
    char *row_column_code;
    char *footnote_type;
    char *footnote_id;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteAssociationMeursingHeadingOplogModel;

typedef struct {
    FootnoteAssociationMeursingHeadingOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteAssociationMeursingHeadingOplogModelList;

#endif
