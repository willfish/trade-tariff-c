#ifndef TRADE_TARIFF_C_EXPLICIT_ABROGATION_REGULATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_EXPLICIT_ABROGATION_REGULATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *explicit_abrogation_regulation_role;
    char *explicit_abrogation_regulation_id;
    char *published_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *replacement_indicator;
    char *abrogation_date;
    char *information_text;
    char *approved_flag;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} ExplicitAbrogationRegulationOplogModel;

typedef struct {
    ExplicitAbrogationRegulationOplogModel *items;
    size_t len;
    size_t cap;
} ExplicitAbrogationRegulationOplogModelList;

#endif
