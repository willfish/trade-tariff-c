#ifndef TRADE_TARIFF_C_MODIFICATION_REGULATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MODIFICATION_REGULATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *modification_regulation_role;
    char *modification_regulation_id;
    char *validity_start_date;
    char *validity_end_date;
    char *published_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *base_regulation_role;
    char *base_regulation_id;
    char *replacement_indicator;
    char *stopped_flag;
    char *information_text;
    char *approved_flag;
    char *explicit_abrogation_regulation_role;
    char *explicit_abrogation_regulation_id;
    char *effective_end_date;
    char *complete_abrogation_regulation_role;
    char *complete_abrogation_regulation_id;
    char *created_at;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} ModificationRegulationOplogModel;

typedef struct {
    ModificationRegulationOplogModel *items;
    size_t len;
    size_t cap;
} ModificationRegulationOplogModelList;

#endif
