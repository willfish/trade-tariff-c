#ifndef TRADE_TARIFF_C_PUBLICATION_SIGLE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_PUBLICATION_SIGLE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *oid;
    char *code_type_id;
    char *code;
    char *publication_code;
    char *publication_sigle;
    char *validity_end_date;
    char *validity_start_date;
    char *operation;
    char *operation_date;
    char *filename;
} PublicationSigleOplogModel;

typedef struct {
    PublicationSigleOplogModel *items;
    size_t len;
    size_t cap;
} PublicationSigleOplogModelList;

#endif
