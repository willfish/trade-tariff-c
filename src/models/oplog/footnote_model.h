#ifndef TRADE_TARIFF_C_FOOTNOTE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *footnote_id;
    char *footnote_type_id;
    char *validity_start_date;
    char *validity_end_date;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteOplogModel;

typedef struct {
    FootnoteOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteOplogModelList;

#endif
