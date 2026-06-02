#include "loaders/commodity_load_plan.h"

#define INCLUDE_ID_COUNT(include_ids) (sizeof(include_ids) / sizeof((include_ids)[0]))

static int plan_needs_root_duty_calculator_meta(void);

static int plan_needs_any(const CommodityIncludePlan *plan,
                          const CommodityIncludeId *include_ids,
                          size_t include_id_count)
{
    return commodity_include_plan_enabled_any(plan, include_ids, include_id_count);
}

static int plan_needs_chapter_guides(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_CHAPTER,
        COMMODITY_INCLUDE_CHAPTER_GUIDES,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_root_relationships(void)
{
    return 1;
}

static int plan_needs_measure_types(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_TYPE,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_TYPE,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_applicable_measures(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
    };
    return plan_needs_root_relationships() ||
           plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_components(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_resolved_measure_components(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
        COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS,
        COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT,
        COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measurement_units(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT,
        COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT,
        COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT,
    };
    return plan_needs_root_duty_calculator_meta() ||
           plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measurement_unit_qualifiers(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT_QUALIFIER,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT_QUALIFIER,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_condition_rendering(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS,
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_COMPONENTS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_COMPONENTS,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_condition_permutation_groups(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS,
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS_PERMUTATIONS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS_PERMUTATIONS,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_conditions(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS,
        COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE,
    };
    return plan_needs_root_duty_calculator_meta() ||
           plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_condition_codes(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_CODE,
        COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_CODE,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_geographical_areas(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_GEOGRAPHICAL_AREA,
        COMMODITY_INCLUDE_IMPORT_MEASURES_GEOGRAPHICAL_AREA_CONTAINED_GEOGRAPHICAL_AREAS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_GEOGRAPHICAL_AREA,
        COMMODITY_INCLUDE_EXPORT_MEASURES_GEOGRAPHICAL_AREA_CONTAINED_GEOGRAPHICAL_AREAS,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_excluded_geographical_areas(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_IMPORT_MEASURES_EXCLUDED_GEOGRAPHICAL_AREAS,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES_EXCLUDED_GEOGRAPHICAL_AREAS,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_additional_codes(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_ADDITIONAL_CODE,
        COMMODITY_INCLUDE_EXPORT_MEASURES_ADDITIONAL_CODE,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_root_duty_calculator_meta(void)
{
    return 1;
}

static int plan_needs_import_trade_summary(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_TRADE_SUMMARY,
    };
    return plan_needs_root_relationships() ||
           plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_footnotes(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
        COMMODITY_INCLUDE_IMPORT_MEASURES_FOOTNOTES,
        COMMODITY_INCLUDE_EXPORT_MEASURES_FOOTNOTES,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_measure_legal_acts(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES,
        COMMODITY_INCLUDE_EXPORT_MEASURES,
        COMMODITY_INCLUDE_IMPORT_MEASURES_LEGAL_ACTS,
        COMMODITY_INCLUDE_EXPORT_MEASURES_LEGAL_ACTS,
        COMMODITY_INCLUDE_IMPORT_MEASURES_SUSPENDING_REGULATION,
        COMMODITY_INCLUDE_EXPORT_MEASURES_SUSPENDING_REGULATION,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_quota_models(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER,
        COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER,
        COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER_DEFINITION,
        COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER_DEFINITION,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

static int plan_needs_quota_closed_and_transferred_events(const CommodityIncludePlan *plan)
{
    static const CommodityIncludeId include_ids[] = {
        COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER_DEFINITION_INCOMING_QUOTA_CLOSED_AND_TRANSFERRED_EVENT,
    };
    return plan_needs_any(plan, include_ids, INCLUDE_ID_COUNT(include_ids));
}

CommodityLoadPlan commodity_load_plan_from_include_plan(const CommodityIncludePlan *include_plan)
{
    if (include_plan == NULL) {
        return (CommodityLoadPlan){ 0 };
    }

    int quota_models = plan_needs_quota_models(include_plan);
    return (CommodityLoadPlan){
        .ancestors = plan_needs_root_relationships(),
        .classification = plan_needs_root_relationships(),
        .chapter_guides = plan_needs_chapter_guides(include_plan),
        .commodity_footnotes = plan_needs_root_relationships(),
        .applicable_measures = plan_needs_applicable_measures(include_plan),
        .measure_components = plan_needs_root_duty_calculator_meta() ||
                              plan_needs_measure_components(include_plan) ||
                              plan_needs_resolved_measure_components(include_plan),
        .measure_types = plan_needs_measure_types(include_plan),
        .resolved_measure_components = plan_needs_resolved_measure_components(include_plan),
        .measurement_units = plan_needs_measurement_units(include_plan),
        .measurement_unit_qualifiers = plan_needs_measurement_unit_qualifiers(include_plan),
        .import_trade_summary = plan_needs_import_trade_summary(include_plan),
        .measure_conditions = plan_needs_measure_conditions(include_plan),
        .measure_condition_permutation_groups = plan_needs_measure_condition_permutation_groups(include_plan),
        .appendix_5as = plan_needs_measure_condition_rendering(include_plan),
        .measure_condition_codes = plan_needs_measure_condition_codes(include_plan),
        .measure_condition_components = plan_needs_measure_condition_rendering(include_plan),
        .geographical_areas = plan_needs_geographical_areas(include_plan),
        .excluded_geographical_areas = plan_needs_excluded_geographical_areas(include_plan),
        .additional_codes = plan_needs_root_duty_calculator_meta() ||
                            plan_needs_additional_codes(include_plan),
        .measure_footnotes = plan_needs_measure_footnotes(include_plan),
        .measure_legal_acts = plan_needs_measure_legal_acts(include_plan),
        .quota_order_numbers = quota_models,
        .quota_definitions = quota_models,
        .quota_events = quota_models,
        .quota_closed_and_transferred_events = plan_needs_quota_closed_and_transferred_events(include_plan),
    };
}

int commodity_load_plan_self_test(void)
{
    CommodityIncludePlan empty = commodity_include_plan_empty();
    CommodityLoadPlan empty_plan = commodity_load_plan_from_include_plan(&empty);
    if (!empty_plan.ancestors ||
        !empty_plan.classification ||
        !empty_plan.commodity_footnotes ||
        !empty_plan.applicable_measures ||
        !empty_plan.measure_components ||
        !empty_plan.measurement_units ||
        !empty_plan.import_trade_summary ||
        !empty_plan.measure_conditions ||
        empty_plan.chapter_guides ||
        empty_plan.measure_types ||
        empty_plan.resolved_measure_components ||
        empty_plan.quota_definitions) {
        return 0;
    }

    CommodityIncludePlan defaults = commodity_include_plan_default();
    CommodityLoadPlan default_plan = commodity_load_plan_from_include_plan(&defaults);
    return default_plan.ancestors &&
           default_plan.classification &&
           default_plan.chapter_guides &&
           default_plan.commodity_footnotes &&
           default_plan.applicable_measures &&
           default_plan.measure_components &&
           default_plan.measure_types &&
           default_plan.resolved_measure_components &&
           default_plan.measurement_units &&
           default_plan.measure_condition_components &&
           default_plan.import_trade_summary &&
           default_plan.measure_conditions &&
           default_plan.measure_condition_permutation_groups &&
           default_plan.geographical_areas &&
           default_plan.excluded_geographical_areas &&
           default_plan.additional_codes &&
           default_plan.measure_footnotes &&
           default_plan.measure_legal_acts &&
           default_plan.quota_order_numbers &&
           default_plan.quota_definitions &&
           default_plan.quota_events;
}
