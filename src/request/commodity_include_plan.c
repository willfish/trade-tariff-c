#include "request/commodity_include_plan.h"

#include <stdio.h>
#include <string.h>

typedef struct {
    CommodityIncludeId id;
    const char *path;
    int default_include;
} CommodityIncludeSpec;

static const CommodityIncludeSpec include_specs[] = {
    { COMMODITY_INCLUDE_SECTION, "section", 1 },
    { COMMODITY_INCLUDE_CHAPTER, "chapter", 1 },
    { COMMODITY_INCLUDE_CHAPTER_GUIDES, "chapter.guides", 1 },
    { COMMODITY_INCLUDE_FOOTNOTES, "footnotes", 1 },
    { COMMODITY_INCLUDE_IMPORT_TRADE_SUMMARY, "import_trade_summary", 1 },
    { COMMODITY_INCLUDE_HEADING, "heading", 1 },
    { COMMODITY_INCLUDE_ANCESTORS, "ancestors", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES, "import_measures", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_DUTY_EXPRESSION, "import_measures.duty_expression", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_TYPE, "import_measures.measure_type", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_LEGAL_ACTS, "import_measures.legal_acts", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_SUSPENDING_REGULATION, "import_measures.suspending_regulation", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS, "import_measures.measure_conditions", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_CODE, "import_measures.measure_conditions.measure_condition_code", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS, "import_measures.measure_condition_permutation_groups", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS_PERMUTATIONS, "import_measures.measure_condition_permutation_groups.permutations", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_COMPONENTS, "import_measures.measure_conditions.measure_condition_components", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS, "import_measures.measure_components", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT, "import_measures.measure_components.measurement_unit", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT_QUALIFIER, "import_measures.measure_components.measurement_unit_qualifier", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS, "import_measures.resolved_measure_components", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT, "import_measures.resolved_measure_components.measurement_unit", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_GEOGRAPHICAL_AREA, "import_measures.geographical_area", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_GEOGRAPHICAL_AREA_CONTAINED_GEOGRAPHICAL_AREAS, "import_measures.geographical_area.contained_geographical_areas", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_EXCLUDED_GEOGRAPHICAL_AREAS, "import_measures.excluded_geographical_areas", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_FOOTNOTES, "import_measures.footnotes", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_ADDITIONAL_CODE, "import_measures.additional_code", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER, "import_measures.order_number", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER_DEFINITION, "import_measures.order_number.definition", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER_DEFINITION_INCOMING_QUOTA_CLOSED_AND_TRANSFERRED_EVENT, "import_measures.order_number.definition.incoming_quota_closed_and_transferred_event", 1 },
    { COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE, "import_measures.preference_code", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES, "export_measures", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_DUTY_EXPRESSION, "export_measures.duty_expression", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_TYPE, "export_measures.measure_type", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_LEGAL_ACTS, "export_measures.legal_acts", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_SUSPENDING_REGULATION, "export_measures.suspending_regulation", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS, "export_measures.measure_conditions", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_CODE, "export_measures.measure_conditions.measure_condition_code", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS, "export_measures.measure_condition_permutation_groups", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS_PERMUTATIONS, "export_measures.measure_condition_permutation_groups.permutations", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_COMPONENTS, "export_measures.measure_conditions.measure_condition_components", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS, "export_measures.measure_components", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT, "export_measures.measure_components.measurement_unit", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT_QUALIFIER, "export_measures.measure_components.measurement_unit_qualifier", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS, "export_measures.resolved_measure_components", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT, "export_measures.resolved_measure_components.measurement_unit", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_GEOGRAPHICAL_AREA, "export_measures.geographical_area", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_GEOGRAPHICAL_AREA_CONTAINED_GEOGRAPHICAL_AREAS, "export_measures.geographical_area.contained_geographical_areas", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_EXCLUDED_GEOGRAPHICAL_AREAS, "export_measures.excluded_geographical_areas", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_FOOTNOTES, "export_measures.footnotes", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_ADDITIONAL_CODE, "export_measures.additional_code", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER, "export_measures.order_number", 1 },
    { COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER_DEFINITION, "export_measures.order_number.definition", 1 },
};

static void enable_include(CommodityIncludePlan *plan, CommodityIncludeId include_id)
{
    if (include_id < COMMODITY_INCLUDE_COUNT) {
        plan->bits |= 1ULL << include_id;
    }
}

CommodityIncludePlan commodity_include_plan_empty(void)
{
    return (CommodityIncludePlan){ .bits = 0 };
}

CommodityIncludePlan commodity_include_plan_default(void)
{
    CommodityIncludePlan plan = commodity_include_plan_empty();
    for (size_t i = 0; i < sizeof(include_specs) / sizeof(include_specs[0]); i++) {
        if (include_specs[i].default_include) {
            enable_include(&plan, include_specs[i].id);
        }
    }
    return plan;
}

int commodity_include_plan_enabled(const CommodityIncludePlan *plan, CommodityIncludeId include_id)
{
    if (plan == NULL || include_id >= COMMODITY_INCLUDE_COUNT) {
        return 0;
    }
    return (plan->bits & (1ULL << include_id)) != 0;
}

int commodity_include_plan_enabled_any(const CommodityIncludePlan *plan,
                                       const CommodityIncludeId *include_ids,
                                       size_t include_id_count)
{
    if (plan == NULL || include_ids == NULL) {
        return 0;
    }

    for (size_t i = 0; i < include_id_count; i++) {
        if (commodity_include_plan_enabled(plan, include_ids[i])) {
            return 1;
        }
    }
    return 0;
}

int commodity_include_plan_equals(const CommodityIncludePlan *left, const CommodityIncludePlan *right)
{
    return left != NULL && right != NULL && left->bits == right->bits;
}

const char *commodity_include_plan_path(CommodityIncludeId include_id)
{
    for (size_t i = 0; i < sizeof(include_specs) / sizeof(include_specs[0]); i++) {
        if (include_specs[i].id == include_id) {
            return include_specs[i].path;
        }
    }
    return NULL;
}

static int find_include(const char *path, size_t path_len, CommodityIncludeId *out)
{
    for (size_t i = 0; i < sizeof(include_specs) / sizeof(include_specs[0]); i++) {
        if (strlen(include_specs[i].path) == path_len &&
            strncmp(include_specs[i].path, path, path_len) == 0) {
            *out = include_specs[i].id;
            return 1;
        }
    }
    return 0;
}

static void copy_invalid(char *dest, size_t dest_len, const char *value, size_t value_len)
{
    if (dest == NULL || dest_len == 0) {
        return;
    }
    size_t copy_len = value_len >= dest_len ? dest_len - 1 : value_len;
    memcpy(dest, value, copy_len);
    dest[copy_len] = '\0';
}

CommodityIncludeParseResult commodity_include_plan_parse(const char *include_param,
                                                         CommodityIncludePlan *out,
                                                         char *invalid_include,
                                                         size_t invalid_include_len)
{
    if (out == NULL) {
        return COMMODITY_INCLUDE_PARSE_UNSUPPORTED;
    }

    *out = commodity_include_plan_empty();
    if (invalid_include != NULL && invalid_include_len > 0) {
        invalid_include[0] = '\0';
    }

    if (include_param == NULL || *include_param == '\0') {
        *out = commodity_include_plan_default();
        return COMMODITY_INCLUDE_PARSE_OK;
    }

    const char *cursor = include_param;
    while (*cursor != '\0') {
        const char *end = strchr(cursor, ',');
        size_t len = end == NULL ? strlen(cursor) : (size_t)(end - cursor);

        if (len > 0) {
            CommodityIncludeId include_id = COMMODITY_INCLUDE_COUNT;
            if (!find_include(cursor, len, &include_id)) {
                copy_invalid(invalid_include, invalid_include_len, cursor, len);
                return COMMODITY_INCLUDE_PARSE_UNSUPPORTED;
            }
            enable_include(out, include_id);
        }

        if (end == NULL) {
            break;
        }
        cursor = end + 1;
    }

    return COMMODITY_INCLUDE_PARSE_OK;
}

int commodity_include_plan_self_test(void)
{
    CommodityIncludePlan plan = commodity_include_plan_default();
    if (!commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_SECTION) ||
        !commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE) ||
        !commodity_include_plan_enabled(&plan, COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT)) {
        return 0;
    }

    CommodityIncludePlan parsed = commodity_include_plan_empty();
    char invalid[32];
    if (commodity_include_plan_parse("section,import_measures.preference_code", &parsed, invalid, sizeof(invalid)) != COMMODITY_INCLUDE_PARSE_OK) {
        return 0;
    }

    if (!commodity_include_plan_enabled(&parsed, COMMODITY_INCLUDE_SECTION) ||
        !commodity_include_plan_enabled(&parsed, COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE) ||
        commodity_include_plan_enabled(&parsed, COMMODITY_INCLUDE_CHAPTER)) {
        return 0;
    }

    if (commodity_include_plan_parse("section,unknown", &parsed, invalid, sizeof(invalid)) != COMMODITY_INCLUDE_PARSE_UNSUPPORTED) {
        return 0;
    }
    return strcmp(invalid, "unknown") == 0;
}
