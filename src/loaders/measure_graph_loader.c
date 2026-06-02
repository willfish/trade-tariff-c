#include "loaders/measure_graph_loader.h"

#include "repositories/additional_code_repository.h"
#include "repositories/appendix_5a_repository.h"
#include "repositories/geographical_area_repository.h"
#include "repositories/measure_component_repository.h"
#include "repositories/measure_condition_code_repository.h"
#include "repositories/measure_condition_component_repository.h"
#include "repositories/measure_condition_repository.h"
#include "repositories/measure_excluded_geographical_area_repository.h"
#include "repositories/measure_footnote_repository.h"
#include "repositories/measure_legal_act_repository.h"
#include "repositories/measure_repository.h"
#include "repositories/measure_type_repository.h"
#include "repositories/measurement_unit_qualifier_repository.h"
#include "repositories/measurement_unit_repository.h"
#include "repositories/quota_closed_and_transferred_event_repository.h"
#include "repositories/quota_definition_repository.h"
#include "repositories/quota_event_repository.h"
#include "repositories/quota_order_number_repository.h"
#include "presenters/measure_condition_permutation_presenter.h"

#include <stdlib.h>

int measure_graph_loader_load_applicable_measures(const CommodityRequestContext *context,
                                                  CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **sids = commodity_aggregate_goods_nomenclature_sids(aggregate, &sid_count);
    if (sids == NULL || sid_count == 0) {
        free(sids);
        return 0;
    }

    MeasureModelList measures = { 0 };
    int ok = measure_repository_fetch_for_goods_nomenclature_sids(context,
                                                                  (const char *const *)sids,
                                                                  sid_count,
                                                                  &measures);
    free(sids);
    if (!ok) {
        measure_model_list_free(&measures);
        return 0;
    }

    return commodity_aggregate_set_applicable_measures(aggregate, measures);
}

int measure_graph_loader_load_measure_components(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_measure_components(aggregate, (MeasureComponentModelList){ 0 });
    }

    MeasureComponentModelList components = { 0 };
    int ok = measure_component_repository_fetch_for_measure_sids(context,
                                                                 (const char *const *)measure_sids,
                                                                 sid_count,
                                                                 &components);
    free(measure_sids);
    if (!ok) {
        measure_component_model_list_free(&components);
        return 0;
    }

    return commodity_aggregate_set_measure_components(aggregate, components);
}

int measure_graph_loader_load_resolved_measure_components(const CommodityRequestContext *context,
                                                          CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_resolved_measure_components(aggregate, (MeasureComponentModelList){ 0 });
    }

    MeasureComponentModelList components = { 0 };
    int ok = measure_component_repository_fetch_resolved_meursing_for_measure_sids(
        context,
        (const char *const *)measure_sids,
        sid_count,
        &components);
    free(measure_sids);
    if (!ok) {
        measure_component_model_list_free(&components);
        return 0;
    }

    return commodity_aggregate_set_resolved_measure_components(aggregate, components);
}

int measure_graph_loader_load_measure_types(const CommodityRequestContext *context,
                                            CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t id_count = 0;
    char **ids = commodity_aggregate_measure_type_ids(aggregate, &id_count);
    if (ids == NULL || id_count == 0) {
        free(ids);
        return commodity_aggregate_set_measure_types(aggregate, (MeasureTypeModelList){ 0 });
    }

    MeasureTypeModelList measure_types = { 0 };
    int ok = measure_type_repository_fetch_for_ids(context,
                                                   (const char *const *)ids,
                                                   id_count,
                                                   &measure_types);
    free(ids);
    if (!ok) {
        measure_type_model_list_free(&measure_types);
        return 0;
    }

    return commodity_aggregate_set_measure_types(aggregate, measure_types);
}

int measure_graph_loader_load_measurement_units(const CommodityRequestContext *context,
                                                CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t code_count = 0;
    char **codes = commodity_aggregate_measurement_unit_codes(aggregate, &code_count);
    if (codes == NULL || code_count == 0) {
        free(codes);
        return commodity_aggregate_set_measurement_units(aggregate, (MeasurementUnitModelList){ 0 });
    }

    MeasurementUnitModelList units = { 0 };
    int ok = measurement_unit_repository_fetch_for_codes(context,
                                                         (const char *const *)codes,
                                                         code_count,
                                                         &units);
    free(codes);
    if (!ok) {
        measurement_unit_model_list_free(&units);
        return 0;
    }

    return commodity_aggregate_set_measurement_units(aggregate, units);
}

int measure_graph_loader_load_measurement_unit_qualifiers(const CommodityRequestContext *context,
                                                          CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t code_count = 0;
    char **codes = commodity_aggregate_measurement_unit_qualifier_codes(aggregate, &code_count);
    if (codes == NULL || code_count == 0) {
        free(codes);
        return commodity_aggregate_set_measurement_unit_qualifiers(aggregate, (MeasurementUnitQualifierModelList){ 0 });
    }

    MeasurementUnitQualifierModelList qualifiers = { 0 };
    int ok = measurement_unit_qualifier_repository_fetch_for_codes(context,
                                                                   (const char *const *)codes,
                                                                   code_count,
                                                                   &qualifiers);
    free(codes);
    if (!ok) {
        measurement_unit_qualifier_model_list_free(&qualifiers);
        return 0;
    }

    return commodity_aggregate_set_measurement_unit_qualifiers(aggregate, qualifiers);
}

int measure_graph_loader_load_measure_conditions(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_measure_conditions(aggregate, (MeasureConditionModelList){ 0 });
    }

    MeasureConditionModelList conditions = { 0 };
    int ok = measure_condition_repository_fetch_for_measure_sids(context,
                                                                 (const char *const *)measure_sids,
                                                                 sid_count,
                                                                 &conditions);
    free(measure_sids);
    if (!ok) {
        measure_condition_model_list_free(&conditions);
        return 0;
    }

    return commodity_aggregate_set_measure_conditions(aggregate, conditions);
}

int measure_graph_loader_load_measure_condition_permutation_groups(const CommodityRequestContext *context,
                                                                   CommodityAggregate *aggregate)
{
    (void)context;
    if (aggregate == NULL) {
        return 0;
    }

    MeasureConditionPermutationGroupModelList groups = { 0 };
    if (!measure_condition_permutation_presenter_build_groups(&aggregate->measure_conditions, &groups)) {
        measure_condition_permutation_group_model_list_free(&groups);
        return 0;
    }
    return commodity_aggregate_set_measure_condition_permutation_groups(aggregate, groups);
}

int measure_graph_loader_load_appendix_5as(const CommodityRequestContext *context,
                                           CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t document_code_count = 0;
    char **document_codes = commodity_aggregate_appendix_5a_document_codes(aggregate, &document_code_count);
    if (document_codes == NULL || document_code_count == 0) {
        free(document_codes);
        return commodity_aggregate_set_appendix_5as(aggregate, (Appendix5aModelList){ 0 });
    }

    Appendix5aModelList appendix_5as = { 0 };
    int ok = appendix_5a_repository_fetch_for_document_codes(context,
                                                             (const char *const *)document_codes,
                                                             document_code_count,
                                                             &appendix_5as);
    for (size_t i = 0; i < document_code_count; i++) {
        free(document_codes[i]);
    }
    free(document_codes);
    if (!ok) {
        appendix_5a_model_list_free(&appendix_5as);
        return 0;
    }

    return commodity_aggregate_set_appendix_5as(aggregate, appendix_5as);
}

int measure_graph_loader_load_measure_condition_codes(const CommodityRequestContext *context,
                                                      CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t code_count = 0;
    char **codes = commodity_aggregate_measure_condition_codes(aggregate, &code_count);
    if (codes == NULL || code_count == 0) {
        free(codes);
        return commodity_aggregate_set_measure_condition_codes(aggregate, (MeasureConditionCodeModelList){ 0 });
    }

    MeasureConditionCodeModelList condition_codes = { 0 };
    int ok = measure_condition_code_repository_fetch_for_codes(context,
                                                               (const char *const *)codes,
                                                               code_count,
                                                               &condition_codes);
    free(codes);
    if (!ok) {
        measure_condition_code_model_list_free(&condition_codes);
        return 0;
    }

    return commodity_aggregate_set_measure_condition_codes(aggregate, condition_codes);
}

int measure_graph_loader_load_measure_condition_components(const CommodityRequestContext *context,
                                                           CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **condition_sids = commodity_aggregate_measure_condition_sids(aggregate, &sid_count);
    if (condition_sids == NULL || sid_count == 0) {
        free(condition_sids);
        return commodity_aggregate_set_measure_condition_components(aggregate, (MeasureConditionComponentModelList){ 0 });
    }

    MeasureConditionComponentModelList components = { 0 };
    int ok = measure_condition_component_repository_fetch_for_measure_condition_sids(
        context,
        (const char *const *)condition_sids,
        sid_count,
        &components);
    free(condition_sids);
    if (!ok) {
        measure_condition_component_model_list_free(&components);
        return 0;
    }

    return commodity_aggregate_set_measure_condition_components(aggregate, components);
}

int measure_graph_loader_load_geographical_areas(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_geographical_areas(aggregate, (GeographicalAreaModelList){ 0 });
    }

    GeographicalAreaModelList areas = { 0 };
    int ok = geographical_area_repository_fetch_for_measure_sids(context,
                                                                 (const char *const *)measure_sids,
                                                                 sid_count,
                                                                 &areas);
    free(measure_sids);
    if (!ok) {
        geographical_area_model_list_free(&areas);
        return 0;
    }

    return commodity_aggregate_set_geographical_areas(aggregate, areas);
}

int measure_graph_loader_load_excluded_geographical_areas(const CommodityRequestContext *context,
                                                          CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_excluded_geographical_areas(aggregate, (MeasureExcludedGeographicalAreaModelList){ 0 });
    }

    MeasureExcludedGeographicalAreaModelList excluded = { 0 };
    int ok = measure_excluded_geographical_area_repository_fetch_for_measure_sids(
        context,
        (const char *const *)measure_sids,
        sid_count,
        &excluded);
    free(measure_sids);
    if (!ok) {
        measure_excluded_geographical_area_model_list_free(&excluded);
        return 0;
    }

    return commodity_aggregate_set_excluded_geographical_areas(aggregate, excluded);
}

int measure_graph_loader_load_additional_codes(const CommodityRequestContext *context,
                                               CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **additional_code_sids = commodity_aggregate_additional_code_sids(aggregate, &sid_count);
    if (additional_code_sids == NULL || sid_count == 0) {
        free(additional_code_sids);
        return commodity_aggregate_set_additional_codes(aggregate, (AdditionalCodeModelList){ 0 });
    }

    AdditionalCodeModelList additional_codes = { 0 };
    int ok = additional_code_repository_fetch_for_additional_code_sids(
        context,
        (const char *const *)additional_code_sids,
        sid_count,
        &additional_codes);
    free(additional_code_sids);
    if (!ok) {
        additional_code_model_list_free(&additional_codes);
        return 0;
    }

    return commodity_aggregate_set_additional_codes(aggregate, additional_codes);
}

int measure_graph_loader_load_measure_footnotes(const CommodityRequestContext *context,
                                                CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_measure_footnotes(aggregate, (FootnoteList){ 0 });
    }

    FootnoteList footnotes = { 0 };
    int ok = measure_footnote_repository_fetch_for_measure_sids(context,
                                                                (const char *const *)measure_sids,
                                                                sid_count,
                                                                &footnotes);
    free(measure_sids);
    if (!ok) {
        footnote_list_free(&footnotes);
        return 0;
    }

    return commodity_aggregate_set_measure_footnotes(aggregate, footnotes);
}

int measure_graph_loader_load_measure_legal_acts(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **measure_sids = commodity_aggregate_measure_sids(aggregate, &sid_count);
    if (measure_sids == NULL || sid_count == 0) {
        free(measure_sids);
        return commodity_aggregate_set_measure_legal_acts(aggregate, (MeasureLegalActModelList){ 0 });
    }

    MeasureLegalActModelList legal_acts = { 0 };
    int ok = measure_legal_act_repository_fetch_for_measure_sids(context,
                                                                 (const char *const *)measure_sids,
                                                                 sid_count,
                                                                 &legal_acts);
    free(measure_sids);
    if (!ok) {
        measure_legal_act_model_list_free(&legal_acts);
        return 0;
    }

    return commodity_aggregate_set_measure_legal_acts(aggregate, legal_acts);
}

int measure_graph_loader_load_quota_order_numbers(const CommodityRequestContext *context,
                                                  CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t order_number_count = 0;
    char **order_numbers = commodity_aggregate_order_numbers(aggregate, &order_number_count);
    if (order_numbers == NULL || order_number_count == 0) {
        free(order_numbers);
        return commodity_aggregate_set_quota_order_numbers(aggregate, (QuotaOrderNumberModelList){ 0 });
    }

    QuotaOrderNumberModelList quota_order_numbers = { 0 };
    int ok = quota_order_number_repository_fetch_for_order_numbers(
        context,
        (const char *const *)order_numbers,
        order_number_count,
        &quota_order_numbers);
    free(order_numbers);
    if (!ok) {
        quota_order_number_model_list_free(&quota_order_numbers);
        return 0;
    }

    return commodity_aggregate_set_quota_order_numbers(aggregate, quota_order_numbers);
}

int measure_graph_loader_load_quota_definitions(const CommodityRequestContext *context,
                                                CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t order_number_count = 0;
    char **order_numbers = commodity_aggregate_order_numbers(aggregate, &order_number_count);
    if (order_numbers == NULL || order_number_count == 0) {
        free(order_numbers);
        return commodity_aggregate_set_quota_definitions(aggregate, (QuotaDefinitionModelList){ 0 });
    }

    QuotaDefinitionModelList definitions = { 0 };
    int ok = quota_definition_repository_fetch_for_order_numbers(
        context,
        (const char *const *)order_numbers,
        order_number_count,
        &definitions);
    free(order_numbers);
    if (!ok) {
        quota_definition_model_list_free(&definitions);
        return 0;
    }

    return commodity_aggregate_set_quota_definitions(aggregate, definitions);
}

int measure_graph_loader_load_quota_events(const CommodityRequestContext *context,
                                           CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **definition_sids = commodity_aggregate_quota_definition_sids(aggregate, &sid_count);
    if (definition_sids == NULL || sid_count == 0) {
        free(definition_sids);
        return commodity_aggregate_set_quota_events(aggregate, (QuotaEventModelList){ 0 });
    }

    QuotaEventModelList events = { 0 };
    int ok = quota_event_repository_fetch_latest_for_definition_sids(
        context,
        (const char *const *)definition_sids,
        sid_count,
        &events);
    free(definition_sids);
    if (!ok) {
        quota_event_model_list_free(&events);
        return 0;
    }

    return commodity_aggregate_set_quota_events(aggregate, events);
}

int measure_graph_loader_load_quota_closed_and_transferred_events(const CommodityRequestContext *context,
                                                                  CommodityAggregate *aggregate)
{
    if (context == NULL || aggregate == NULL) {
        return 0;
    }

    size_t sid_count = 0;
    char **definition_sids = commodity_aggregate_quota_definition_sids(aggregate, &sid_count);
    if (definition_sids == NULL || sid_count == 0) {
        free(definition_sids);
        return commodity_aggregate_set_quota_closed_and_transferred_events(
            aggregate,
            (QuotaClosedAndTransferredEventModelList){ 0 });
    }

    QuotaClosedAndTransferredEventModelList events = { 0 };
    int ok = quota_closed_and_transferred_event_repository_fetch_for_target_definition_sids(
        context,
        (const char *const *)definition_sids,
        sid_count,
        &events);
    free(definition_sids);
    if (!ok) {
        quota_closed_and_transferred_event_model_list_free(&events);
        return 0;
    }

    return commodity_aggregate_set_quota_closed_and_transferred_events(aggregate, events);
}

int measure_graph_loader_self_test(void)
{
    return measure_graph_loader_load_applicable_measures(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_types(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_components(NULL, NULL) == 0 &&
           measure_graph_loader_load_resolved_measure_components(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_conditions(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_condition_codes(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_condition_components(NULL, NULL) == 0 &&
           measure_graph_loader_load_geographical_areas(NULL, NULL) == 0 &&
           measure_graph_loader_load_excluded_geographical_areas(NULL, NULL) == 0 &&
           measure_graph_loader_load_additional_codes(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_footnotes(NULL, NULL) == 0 &&
           measure_graph_loader_load_measure_legal_acts(NULL, NULL) == 0 &&
           measure_graph_loader_load_quota_order_numbers(NULL, NULL) == 0 &&
           measure_graph_loader_load_quota_definitions(NULL, NULL) == 0 &&
           measure_graph_loader_load_quota_events(NULL, NULL) == 0 &&
           measure_graph_loader_load_quota_closed_and_transferred_events(NULL, NULL) == 0;
}
