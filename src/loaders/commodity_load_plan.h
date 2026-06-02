#ifndef TRADE_TARIFF_C_COMMODITY_LOAD_PLAN_H
#define TRADE_TARIFF_C_COMMODITY_LOAD_PLAN_H

#include "request/commodity_include_plan.h"

typedef struct {
    int ancestors;
    int classification;
    int chapter_guides;
    int commodity_footnotes;
    int applicable_measures;
    int measure_components;
    int measure_types;
    int resolved_measure_components;
    int measurement_units;
    int measurement_unit_qualifiers;
    int import_trade_summary;
    int measure_conditions;
    int measure_condition_permutation_groups;
    int appendix_5as;
    int measure_condition_codes;
    int measure_condition_components;
    int geographical_areas;
    int excluded_geographical_areas;
    int additional_codes;
    int measure_footnotes;
    int measure_legal_acts;
    int quota_order_numbers;
    int quota_definitions;
    int quota_events;
    int quota_closed_and_transferred_events;
} CommodityLoadPlan;

CommodityLoadPlan commodity_load_plan_from_include_plan(const CommodityIncludePlan *include_plan);
int commodity_load_plan_self_test(void);

#endif
