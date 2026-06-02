#ifndef TRADE_TARIFF_C_COMPLETE_ABROGATION_REGULATION_OPLOG_MODEL_H
#define TRADE_TARIFF_C_COMPLETE_ABROGATION_REGULATION_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *complete_abrogation_regulation_role;
    char *complete_abrogation_regulation_id;
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
} CompleteAbrogationRegulationOplogModel;

typedef struct {
    CompleteAbrogationRegulationOplogModel *items;
    size_t len;
    size_t cap;
} CompleteAbrogationRegulationOplogModelList;

#endif
