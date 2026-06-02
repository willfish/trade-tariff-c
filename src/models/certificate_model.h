#ifndef TRADE_TARIFF_C_CERTIFICATE_MODEL_H
#define TRADE_TARIFF_C_CERTIFICATE_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_type_code;
    char *certificate_code;
    char *description;
} CertificateModel;

typedef struct {
    CertificateModel *items;
    size_t len;
    size_t cap;
} CertificateModelList;

#endif
