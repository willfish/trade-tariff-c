#ifndef TRADE_TARIFF_C_COMMODITY_PRESENTER_H
#define TRADE_TARIFF_C_COMMODITY_PRESENTER_H

#include "domain/commodity_aggregate.h"
#include "domain/goods_nomenclature_tree.h"
#include "models/commodity_model.h"
#include "models/duty_calculator_metadata_model.h"

const char *commodity_presenter_resource_type(void);
const char *commodity_presenter_id(const CommodityModel *model);
const char *commodity_presenter_tree_node_id(const GoodsNomenclatureTreeNode *node);
char *commodity_presenter_basic_duty_rate(const CommodityAggregate *aggregate);
int commodity_presenter_meursing_code(const CommodityAggregate *aggregate);
int commodity_presenter_zero_mfn_duty(const CommodityAggregate *aggregate);
int commodity_presenter_trade_defence(const CommodityAggregate *aggregate);
int commodity_presenter_entry_price_system(const CommodityAggregate *aggregate,
                                           const char *service);
DutyCalculatorAdditionalCodeGroupModelList commodity_presenter_applicable_additional_codes(const CommodityAggregate *aggregate);
DutyCalculatorVatOptionModelList commodity_presenter_applicable_vat_options(const CommodityAggregate *aggregate);
DutyCalculatorMeasureUnitModelList commodity_presenter_applicable_measure_units(const CommodityAggregate *aggregate);
int commodity_presenter_self_test(void);

#endif
