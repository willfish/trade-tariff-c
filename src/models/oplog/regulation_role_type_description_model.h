#ifndef TRADE_TARIFF_C_REGULATION_ROLE_TYPE_DESCRIPTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_REGULATION_ROLE_TYPE_DESCRIPTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *regulation_role_type_id;
    char *language_id;
    char *description;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} RegulationRoleTypeDescriptionOplogModel;

typedef struct {
    RegulationRoleTypeDescriptionOplogModel *items;
    size_t len;
    size_t cap;
} RegulationRoleTypeDescriptionOplogModelList;

#endif
