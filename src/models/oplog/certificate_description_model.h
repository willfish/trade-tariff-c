#ifndef TRADE_TARIFF_C_CERTIFICATE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_CERTIFICATE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_description_period_sid;
    char *language_id;
    char *certificate_type_code;
    char *certificate_code;
    char *description;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} CertificateDescriptionOplogModel;

typedef struct {
    CertificateDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} CertificateDescriptionOplogModelList;

#endif
