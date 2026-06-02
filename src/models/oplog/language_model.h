#ifndef TRADE_TARIFF_C_LANGUAGE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_LANGUAGE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *language_id;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} LanguageOplogModel;

typedef struct {
    LanguageOplogModel *items;
    size_t len;
    size_t cap;
} LanguageOplogModelList;

#endif
