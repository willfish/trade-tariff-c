#ifndef TRADE_TARIFF_C_FOOTNOTE_TYPE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_TYPE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *footnote_type_id;
    char *language_id;
    char *description;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteTypeDescriptionOplogModel;

typedef struct {
    FootnoteTypeDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteTypeDescriptionOplogModelList;

#endif
