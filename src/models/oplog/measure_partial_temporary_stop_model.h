#ifndef TRADE_TARIFF_C_MEASURE_PARTIAL_TEMPORARY_STOP_OPLOG_MODEL_H
#define TRADE_TARIFF_C_MEASURE_PARTIAL_TEMPORARY_STOP_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *validity_start_date;
    char *validity_end_date;
    char *partial_temporary_stop_regulation_id;
    char *partial_temporary_stop_regulation_officialjournal_number;
    char *partial_temporary_stop_regulation_officialjournal_page;
    char *abrogation_regulation_id;
    char *abrogation_regulation_officialjournal_number;
    char *abrogation_regulation_officialjournal_page;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} MeasurePartialTemporaryStopOplogModel;

typedef struct {
    MeasurePartialTemporaryStopOplogModel *items;
    size_t len;
    size_t cap;
} MeasurePartialTemporaryStopOplogModelList;

#endif
