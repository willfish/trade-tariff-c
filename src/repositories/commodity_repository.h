#ifndef TRADE_TARIFF_C_COMMODITY_REPOSITORY_H
#define TRADE_TARIFF_C_COMMODITY_REPOSITORY_H

#include "db.h"
#include "models/measure_component_model.h"

typedef struct {
    const char *schema;
    const char *actual_date;
    const char *commodity_id;
} CommodityRepositoryQuery;

DbRows commodity_repository_fetch_measure_components(const char *schema, const char *measure_sid);
int commodity_repository_hydrate_measure_component_row(const DbRows *rows,
                                                       int row,
                                                       MeasureComponentModel *model);
void commodity_repository_measure_component_free(MeasureComponentModel *model);
int commodity_repository_self_test(void);

#endif
