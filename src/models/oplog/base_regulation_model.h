#ifndef TRADE_TARIFF_C_BASE_REGULATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_BASE_REGULATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *base_regulation_role;
    char *base_regulation_id;
    char *validity_start_date;
    char *validity_end_date;
    char *community_code;
    char *regulation_group_id;
    char *replacement_indicator;
    char *stopped_flag;
    char *information_text;
    char *approved_flag;
    char *published_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *effective_end_date;
    char *antidumping_regulation_role;
    char *related_antidumping_regulation_id;
    char *complete_abrogation_regulation_role;
    char *complete_abrogation_regulation_id;
    char *explicit_abrogation_regulation_role;
    char *explicit_abrogation_regulation_id;
    char *created_at;
    char *national;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} BaseRegulationOplogModel;

typedef struct {
    BaseRegulationOplogModel *items;
    size_t len;
    size_t cap;
} BaseRegulationOplogModelList;

#endif
