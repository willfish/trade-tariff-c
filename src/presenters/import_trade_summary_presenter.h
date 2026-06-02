#ifndef TRADE_TARIFF_C_IMPORT_TRADE_SUMMARY_PRESENTER_H
#define TRADE_TARIFF_C_IMPORT_TRADE_SUMMARY_PRESENTER_H

#include "models/import_trade_summary_model.h"
#include "models/measure_component_model.h"
#include "models/measure_model.h"

const char *import_trade_summary_presenter_resource_type(void);
const char *import_trade_summary_presenter_id(const ImportTradeSummaryModel *model);
int import_trade_summary_presenter_build(const MeasureModelList *measures,
                                         const MeasureComponentModelList *components,
                                         ImportTradeSummaryModel *out);
int import_trade_summary_presenter_self_test(void);

#endif
