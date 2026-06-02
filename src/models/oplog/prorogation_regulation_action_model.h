#ifndef TRADE_TARIFF_C_PROROGATION_REGULATION_ACTION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_PROROGATION_REGULATION_ACTION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *prorogation_regulation_role;
    char *prorogation_regulation_id;
    char *prorogated_regulation_role;
    char *prorogated_regulation_id;
    char *prorogated_date;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} ProrogationRegulationActionOplogModel;

typedef struct {
    ProrogationRegulationActionOplogModel *items;
    size_t len;
    size_t cap;
} ProrogationRegulationActionOplogModelList;

#endif
