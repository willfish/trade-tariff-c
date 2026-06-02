#ifndef TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_GOODS_NOMENCLATURE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_GOODS_NOMENCLATURE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *goods_nomenclature_sid;
    char *footnote_type;
    char *footnote_id;
    char *validity_start_date;
    char *validity_end_date;
    char *goods_nomenclature_item_id;
    char *productline_suffix;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteAssociationGoodsNomenclatureOplogModel;

typedef struct {
    FootnoteAssociationGoodsNomenclatureOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteAssociationGoodsNomenclatureOplogModelList;

#endif
