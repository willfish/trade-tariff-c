#ifndef TRADE_TARIFF_C_PROROGATION_REGULATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_PROROGATION_REGULATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *prorogation_regulation_role;
    char *prorogation_regulation_id;
    char *published_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *replacement_indicator;
    char *information_text;
    char *approved_flag;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} ProrogationRegulationOplogModel;

typedef struct {
    ProrogationRegulationOplogModel *items;
    size_t len;
    size_t cap;
} ProrogationRegulationOplogModelList;

#endif
