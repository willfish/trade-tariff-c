#ifndef TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_MEASURE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_MEASURE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *footnote_type_id;
    char *footnote_id;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteAssociationMeasureOplogModel;

typedef struct {
    FootnoteAssociationMeasureOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteAssociationMeasureOplogModelList;

#endif
