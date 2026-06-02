#ifndef TRADE_TARIFF_C_CERTIFICATE_DESCRIPTION_PERIOD_OPLOG_MODEL_H
#define TRADE_TARIFF_C_CERTIFICATE_DESCRIPTION_PERIOD_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_description_period_sid;
    char *certificate_type_code;
    char *certificate_code;
    char *validity_start_date;
    char *validity_end_date;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} CertificateDescriptionPeriodOplogModel;

typedef struct {
    CertificateDescriptionPeriodOplogModel *items;
    size_t len;
    size_t cap;
} CertificateDescriptionPeriodOplogModelList;

#endif
