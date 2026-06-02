#ifndef TRADE_TARIFF_C_CERTIFICATE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_CERTIFICATE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_type_code;
    char *certificate_code;
    char *validity_start_date;
    char *validity_end_date;
    char *national;
    char *national_abbrev;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} CertificateOplogModel;

typedef struct {
    CertificateOplogModel *items;
    size_t len;
    size_t cap;
} CertificateOplogModelList;

#endif
