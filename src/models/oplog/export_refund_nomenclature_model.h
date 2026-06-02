#ifndef TRADE_TARIFF_C_EXPORT_REFUND_NOMENCLATURE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_EXPORT_REFUND_NOMENCLATURE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *export_refund_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *additional_code_type;
    char *export_refund_code;
    char *productline_suffix;
    char *validity_start_date;
    char *validity_end_date;
    char *goods_nomenclature_sid;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} ExportRefundNomenclatureOplogModel;

typedef struct {
    ExportRefundNomenclatureOplogModel *items;
    size_t len;
    size_t cap;
} ExportRefundNomenclatureOplogModelList;

#endif
