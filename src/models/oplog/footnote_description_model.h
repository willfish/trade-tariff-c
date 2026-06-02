#ifndef TRADE_TARIFF_C_FOOTNOTE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *footnote_description_period_sid;
    char *footnote_type_id;
    char *footnote_id;
    char *language_id;
    char *description;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteDescriptionOplogModel;

typedef struct {
    FootnoteDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteDescriptionOplogModelList;

#endif
