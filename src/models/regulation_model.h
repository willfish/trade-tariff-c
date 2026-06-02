#ifndef TRADE_TARIFF_C_REGULATION_MODEL_H
#define TRADE_TARIFF_C_REGULATION_MODEL_H

#include <stddef.h>

typedef struct {
    char *regulation_id;
    char *regulation_role;
    char *information_text;
    char *validity_start_date;
    char *validity_end_date;
    char *officialjournal_number;
    char *officialjournal_page;
    char *published_date;
} RegulationModel;

typedef struct {
    RegulationModel *items;
    size_t len;
    size_t cap;
} RegulationModelList;

#endif
