#ifndef TRADE_TARIFF_C_MEASURE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *measure_type_id;
    char *geographical_area_id;
    char *goods_nomenclature_item_id;
    char *validity_start_date;
    char *validity_end_date;
    char *measure_generating_regulation_role;
    char *measure_generating_regulation_id;
    char *justification_regulation_role;
    char *justification_regulation_id;
    char *stopped_flag;
    char *geographical_area_sid;
    char *goods_nomenclature_sid;
    char *ordernumber;
    char *additional_code_type_id;
    char *additional_code_id;
    char *additional_code_sid;
    char *reduction_indicator;
    char *export_refund_nomenclature_sid;
    char *created_at;
    char *national;
    char *tariff_measure_number;
    char *invalidated_by;
    char *invalidated_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasureOplogModel;

typedef struct {
    MeasureOplogModel *items;
    size_t len;
    size_t cap;
} MeasureOplogModelList;

#endif
