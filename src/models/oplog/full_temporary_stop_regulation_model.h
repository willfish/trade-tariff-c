#ifndef TRADE_TARIFF_C_FULL_TEMPORARY_STOP_REGULATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_FULL_TEMPORARY_STOP_REGULATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *full_temporary_stop_regulation_role;
    char *full_temporary_stop_regulation_id;
    char *published_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *validity_start_date;
    char *validity_end_date;
    char *effective_enddate;
    char *explicit_abrogation_regulation_role;
    char *explicit_abrogation_regulation_id;
    char *replacement_indicator;
    char *information_text;
    char *approved_flag;
    char *oid;
    char *operation;
    char *operation_date;
    char *complete_abrogation_regulation_role;
    char *complete_abrogation_regulation_id;
    char *filename;
} FullTemporaryStopRegulationOplogModel;

typedef struct {
    FullTemporaryStopRegulationOplogModel *items;
    size_t len;
    size_t cap;
} FullTemporaryStopRegulationOplogModelList;

#endif
