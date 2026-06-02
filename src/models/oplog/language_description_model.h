#ifndef TRADE_TARIFF_C_LANGUAGE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_LANGUAGE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *language_code_id;
    char *language_id;
    char *description;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} LanguageDescriptionOplogModel;

typedef struct {
    LanguageDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} LanguageDescriptionOplogModelList;

#endif
