#ifndef TRADE_TARIFF_C_FOOTNOTE_TYPE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_TYPE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *footnote_type_id;
    char *application_code;
    char *validity_start_date;
    char *validity_end_date;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteTypeOplogModel;

typedef struct {
    FootnoteTypeOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteTypeOplogModelList;

#endif
