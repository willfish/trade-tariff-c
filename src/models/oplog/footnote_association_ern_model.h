#ifndef TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_ERN_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_ERN_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *export_refund_nomenclature_sid;
    char *footnote_type;
    char *footnote_id;
    char *validity_start_date;
    char *validity_end_date;
    char *goods_nomenclature_item_id;
    char *additional_code_type;
    char *export_refund_code;
    char *productline_suffix;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteAssociationErnOplogModel;

typedef struct {
    FootnoteAssociationErnOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteAssociationErnOplogModelList;

#endif
