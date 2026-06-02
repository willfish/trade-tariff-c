#ifndef TRADE_TARIFF_C_MEURSING_ADDITIONAL_CODE_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEURSING_ADDITIONAL_CODE_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *meursing_additional_code_sid;
    char *additional_code;
    char *validity_start_date;
    char *validity_end_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeursingAdditionalCodeOplogModel;

typedef struct {
    MeursingAdditionalCodeOplogModel *items;
    size_t len;
    size_t cap;
} MeursingAdditionalCodeOplogModelList;

#endif
