#ifndef TRADE_TARIFF_C_CERTIFICATE_TYPE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_CERTIFICATE_TYPE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_type_code;
    char *language_id;
    char *description;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} CertificateTypeDescriptionOplogModel;

typedef struct {
    CertificateTypeDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} CertificateTypeDescriptionOplogModelList;

#endif
