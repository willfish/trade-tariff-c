#ifndef TRADE_TARIFF_C_CERTIFICATE_TYPE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_CERTIFICATE_TYPE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_type_code;
    char *validity_start_date;
    char *validity_end_date;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} CertificateTypeOplogModel;

typedef struct {
    CertificateTypeOplogModel *items;
    size_t len;
    size_t cap;
} CertificateTypeOplogModelList;

#endif
