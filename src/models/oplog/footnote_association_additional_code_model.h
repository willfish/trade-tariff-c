#ifndef TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_ADDITIONAL_CODE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_ASSOCIATION_ADDITIONAL_CODE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *additional_code_sid;
    char *footnote_type_id;
    char *footnote_id;
    char *validity_start_date;
    char *validity_end_date;
    char *additional_code_type_id;
    char *additional_code;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} FootnoteAssociationAdditionalCodeOplogModel;

typedef struct {
    FootnoteAssociationAdditionalCodeOplogModel *items;
    size_t len;
    size_t cap;
} FootnoteAssociationAdditionalCodeOplogModelList;

#endif
