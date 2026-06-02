#ifndef TRADE_TARIFF_C_EXPORT_REFUND_NOMENCLATURE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_EXPORT_REFUND_NOMENCLATURE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *export_refund_nomenclature_description_period_sid;
    char *language_id;
    char *export_refund_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *additional_code_type;
    char *export_refund_code;
    char *productline_suffix;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} ExportRefundNomenclatureDescriptionOplogModel;

typedef struct {
    ExportRefundNomenclatureDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} ExportRefundNomenclatureDescriptionOplogModelList;

#endif
