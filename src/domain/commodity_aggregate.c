#include "domain/commodity_aggregate.h"

#include <stdlib.h>
#include <string.h>

void commodity_aggregate_free(CommodityAggregate *aggregate)
{
    if (aggregate == NULL) {
        return;
    }

    goods_nomenclature_tree_node_free(&aggregate->commodity);
    goods_nomenclature_tree_node_list_free(&aggregate->ancestors);
    classification_model_free(&aggregate->classification);
    guide_model_list_free(&aggregate->chapter_guides);
    import_trade_summary_model_free(&aggregate->import_trade_summary);
    measure_model_list_free(&aggregate->applicable_measures);
    measure_type_model_list_free(&aggregate->measure_types);
    measure_component_model_list_free(&aggregate->measure_components);
    measure_component_model_list_free(&aggregate->resolved_measure_components);
    measurement_unit_model_list_free(&aggregate->measurement_units);
    measurement_unit_qualifier_model_list_free(&aggregate->measurement_unit_qualifiers);
    measure_condition_model_list_free(&aggregate->measure_conditions);
    measure_condition_permutation_group_model_list_free(&aggregate->measure_condition_permutation_groups);
    appendix_5a_model_list_free(&aggregate->appendix_5as);
    measure_condition_code_model_list_free(&aggregate->measure_condition_codes);
    measure_condition_component_model_list_free(&aggregate->measure_condition_components);
    geographical_area_model_list_free(&aggregate->geographical_areas);
    measure_excluded_geographical_area_model_list_free(&aggregate->excluded_geographical_areas);
    additional_code_model_list_free(&aggregate->additional_codes);
    footnote_list_free(&aggregate->commodity_footnotes);
    footnote_list_free(&aggregate->measure_footnotes);
    measure_legal_act_model_list_free(&aggregate->measure_legal_acts);
    quota_order_number_model_list_free(&aggregate->quota_order_numbers);
    quota_definition_model_list_free(&aggregate->quota_definitions);
    quota_event_model_list_free(&aggregate->quota_events);
    quota_closed_and_transferred_event_model_list_free(&aggregate->quota_closed_and_transferred_events);
}

char **commodity_aggregate_goods_nomenclature_sids(const CommodityAggregate *aggregate,
                                                   size_t *sid_count)
{
    if (sid_count != NULL) {
        *sid_count = 0;
    }
    if (aggregate == NULL || sid_count == NULL || aggregate->commodity.goods_nomenclature_sid == NULL) {
        return NULL;
    }

    size_t count = aggregate->ancestors.len + 1;
    char **sids = calloc(count, sizeof(*sids));
    if (sids == NULL) {
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < aggregate->ancestors.len; i++) {
        sids[index++] = aggregate->ancestors.items[i].goods_nomenclature_sid;
    }
    sids[index++] = aggregate->commodity.goods_nomenclature_sid;
    *sid_count = index;
    return sids;
}

int commodity_aggregate_set_applicable_measures(CommodityAggregate *aggregate,
                                                MeasureModelList measures)
{
    if (aggregate == NULL) {
        measure_model_list_free(&measures);
        return 0;
    }

    measure_model_list_free(&aggregate->applicable_measures);
    aggregate->applicable_measures = measures;
    return 1;
}

char **commodity_aggregate_measure_type_ids(const CommodityAggregate *aggregate,
                                            size_t *id_count)
{
    if (id_count != NULL) {
        *id_count = 0;
    }
    if (aggregate == NULL || id_count == NULL || aggregate->applicable_measures.len == 0) {
        return NULL;
    }

    char **ids = calloc(aggregate->applicable_measures.len, sizeof(*ids));
    if (ids == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const char *id = aggregate->applicable_measures.items[i].measure_type_id;
        if (id == NULL || *id == '\0') {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *id_count; j++) {
            if (strcmp(ids[j], id) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            ids[*id_count] = (char *)id;
            (*id_count)++;
        }
    }

    if (*id_count == 0) {
        free(ids);
        return NULL;
    }
    return ids;
}

int commodity_aggregate_set_measure_types(CommodityAggregate *aggregate,
                                          MeasureTypeModelList measure_types)
{
    if (aggregate == NULL) {
        measure_type_model_list_free(&measure_types);
        return 0;
    }

    measure_type_model_list_free(&aggregate->measure_types);
    aggregate->measure_types = measure_types;
    return 1;
}

int commodity_aggregate_set_classification(CommodityAggregate *aggregate,
                                           ClassificationModel classification)
{
    if (aggregate == NULL) {
        classification_model_free(&classification);
        return 0;
    }

    classification_model_free(&aggregate->classification);
    aggregate->classification = classification;
    return 1;
}

int commodity_aggregate_set_chapter_guides(CommodityAggregate *aggregate,
                                           GuideModelList guides)
{
    if (aggregate == NULL) {
        guide_model_list_free(&guides);
        return 0;
    }

    guide_model_list_free(&aggregate->chapter_guides);
    aggregate->chapter_guides = guides;
    return 1;
}

int commodity_aggregate_set_import_trade_summary(CommodityAggregate *aggregate,
                                                 ImportTradeSummaryModel import_trade_summary)
{
    if (aggregate == NULL) {
        import_trade_summary_model_free(&import_trade_summary);
        return 0;
    }

    import_trade_summary_model_free(&aggregate->import_trade_summary);
    aggregate->import_trade_summary = import_trade_summary;
    return 1;
}

char **commodity_aggregate_measure_sids(const CommodityAggregate *aggregate,
                                        size_t *sid_count)
{
    if (sid_count != NULL) {
        *sid_count = 0;
    }
    if (aggregate == NULL || sid_count == NULL || aggregate->applicable_measures.len == 0) {
        return NULL;
    }

    char **sids = calloc(aggregate->applicable_measures.len, sizeof(*sids));
    if (sids == NULL) {
        return NULL;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        if (aggregate->applicable_measures.items[i].measure_sid == NULL) {
            continue;
        }
        sids[count++] = aggregate->applicable_measures.items[i].measure_sid;
    }

    if (count == 0) {
        free(sids);
        return NULL;
    }

    *sid_count = count;
    return sids;
}

int commodity_aggregate_set_measure_components(CommodityAggregate *aggregate,
                                               MeasureComponentModelList components)
{
    if (aggregate == NULL) {
        measure_component_model_list_free(&components);
        return 0;
    }

    measure_component_model_list_free(&aggregate->measure_components);
    aggregate->measure_components = components;
    return 1;
}

int commodity_aggregate_set_resolved_measure_components(CommodityAggregate *aggregate,
                                                        MeasureComponentModelList components)
{
    if (aggregate == NULL) {
        measure_component_model_list_free(&components);
        return 0;
    }

    measure_component_model_list_free(&aggregate->resolved_measure_components);
    aggregate->resolved_measure_components = components;
    return 1;
}

size_t commodity_aggregate_measure_component_count_for_measure(const CommodityAggregate *aggregate,
                                                               const char *measure_sid)
{
    if (aggregate == NULL || measure_sid == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->measure_components.len; i++) {
        if (aggregate->measure_components.items[i].measure_sid != NULL &&
            strcmp(aggregate->measure_components.items[i].measure_sid, measure_sid) == 0) {
            count++;
        }
    }
    return count;
}

static int append_unique_component_codes(char **codes,
                                         size_t *code_count,
                                         const MeasureComponentModelList *components,
                                         int qualifiers)
{
    if (codes == NULL || code_count == NULL || components == NULL) {
        return 0;
    }

    for (size_t i = 0; i < components->len; i++) {
        const char *code = qualifiers
                               ? components->items[i].measurement_unit_qualifier_code
                               : components->items[i].measurement_unit_code;
        if (code == NULL || *code == '\0') {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *code_count; j++) {
            if (strcmp(codes[j], code) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            codes[*code_count] = (char *)code;
            (*code_count)++;
        }
    }
    return 1;
}

static char **collect_unique_component_codes(const CommodityAggregate *aggregate,
                                             size_t *code_count,
                                             int qualifiers)
{
    if (code_count != NULL) {
        *code_count = 0;
    }
    if (aggregate == NULL || code_count == NULL) {
        return NULL;
    }

    size_t cap = aggregate->measure_components.len + aggregate->resolved_measure_components.len;
    if (cap == 0) {
        return NULL;
    }

    char **codes = calloc(cap, sizeof(*codes));
    if (codes == NULL) {
        return NULL;
    }

    if (!append_unique_component_codes(codes, code_count, &aggregate->measure_components, qualifiers) ||
        !append_unique_component_codes(codes, code_count, &aggregate->resolved_measure_components, qualifiers)) {
        free(codes);
        *code_count = 0;
        return NULL;
    }

    if (*code_count == 0) {
        free(codes);
        return NULL;
    }
    return codes;
}

char **commodity_aggregate_measurement_unit_codes(const CommodityAggregate *aggregate,
                                                  size_t *code_count)
{
    if (aggregate == NULL) {
        if (code_count != NULL) {
            *code_count = 0;
        }
        return NULL;
    }
    return collect_unique_component_codes(aggregate, code_count, 0);
}

char **commodity_aggregate_measurement_unit_qualifier_codes(const CommodityAggregate *aggregate,
                                                            size_t *code_count)
{
    if (aggregate == NULL) {
        if (code_count != NULL) {
            *code_count = 0;
        }
        return NULL;
    }
    return collect_unique_component_codes(aggregate, code_count, 1);
}

int commodity_aggregate_set_measurement_units(CommodityAggregate *aggregate,
                                              MeasurementUnitModelList units)
{
    if (aggregate == NULL) {
        measurement_unit_model_list_free(&units);
        return 0;
    }

    measurement_unit_model_list_free(&aggregate->measurement_units);
    aggregate->measurement_units = units;
    return 1;
}

int commodity_aggregate_set_measurement_unit_qualifiers(CommodityAggregate *aggregate,
                                                        MeasurementUnitQualifierModelList qualifiers)
{
    if (aggregate == NULL) {
        measurement_unit_qualifier_model_list_free(&qualifiers);
        return 0;
    }

    measurement_unit_qualifier_model_list_free(&aggregate->measurement_unit_qualifiers);
    aggregate->measurement_unit_qualifiers = qualifiers;
    return 1;
}

int commodity_aggregate_set_measure_conditions(CommodityAggregate *aggregate,
                                               MeasureConditionModelList conditions)
{
    if (aggregate == NULL) {
        measure_condition_model_list_free(&conditions);
        return 0;
    }

    measure_condition_model_list_free(&aggregate->measure_conditions);
    aggregate->measure_conditions = conditions;
    return 1;
}

int commodity_aggregate_set_measure_condition_permutation_groups(
    CommodityAggregate *aggregate,
    MeasureConditionPermutationGroupModelList groups)
{
    if (aggregate == NULL) {
        measure_condition_permutation_group_model_list_free(&groups);
        return 0;
    }

    measure_condition_permutation_group_model_list_free(&aggregate->measure_condition_permutation_groups);
    aggregate->measure_condition_permutation_groups = groups;
    return 1;
}

static char *measure_condition_document_code(const MeasureConditionModel *condition)
{
    if (condition == NULL ||
        condition->certificate_type_code == NULL ||
        condition->certificate_code == NULL ||
        condition->certificate_type_code[0] == '\0' ||
        condition->certificate_code[0] == '\0') {
        return NULL;
    }

    size_t len = strlen(condition->certificate_type_code) + strlen(condition->certificate_code) + 1;
    char *document_code = malloc(len);
    if (document_code == NULL) {
        return NULL;
    }
    memcpy(document_code, condition->certificate_type_code, strlen(condition->certificate_type_code));
    memcpy(document_code + strlen(condition->certificate_type_code), condition->certificate_code, strlen(condition->certificate_code) + 1);
    return document_code;
}

char **commodity_aggregate_appendix_5a_document_codes(const CommodityAggregate *aggregate,
                                                      size_t *document_code_count)
{
    if (document_code_count != NULL) {
        *document_code_count = 0;
    }
    if (aggregate == NULL || document_code_count == NULL || aggregate->measure_conditions.len == 0) {
        return NULL;
    }

    char **document_codes = calloc(aggregate->measure_conditions.len, sizeof(*document_codes));
    if (document_codes == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->measure_conditions.len; i++) {
        char *document_code = measure_condition_document_code(&aggregate->measure_conditions.items[i]);
        if (document_code == NULL) {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *document_code_count; j++) {
            if (strcmp(document_codes[j], document_code) == 0) {
                seen = 1;
                break;
            }
        }
        if (seen) {
            free(document_code);
            continue;
        }
        document_codes[*document_code_count] = document_code;
        (*document_code_count)++;
    }

    if (*document_code_count == 0) {
        free(document_codes);
        return NULL;
    }
    return document_codes;
}

int commodity_aggregate_set_appendix_5as(CommodityAggregate *aggregate,
                                         Appendix5aModelList appendix_5as)
{
    if (aggregate == NULL) {
        appendix_5a_model_list_free(&appendix_5as);
        return 0;
    }

    appendix_5a_model_list_free(&aggregate->appendix_5as);
    aggregate->appendix_5as = appendix_5as;
    return 1;
}

char **commodity_aggregate_measure_condition_codes(const CommodityAggregate *aggregate,
                                                   size_t *code_count)
{
    if (code_count != NULL) {
        *code_count = 0;
    }
    if (aggregate == NULL || code_count == NULL || aggregate->measure_conditions.len == 0) {
        return NULL;
    }

    char **codes = calloc(aggregate->measure_conditions.len, sizeof(*codes));
    if (codes == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->measure_conditions.len; i++) {
        const char *code = aggregate->measure_conditions.items[i].condition_code;
        if (code == NULL || *code == '\0') {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *code_count; j++) {
            if (strcmp(codes[j], code) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            codes[*code_count] = (char *)code;
            (*code_count)++;
        }
    }

    if (*code_count == 0) {
        free(codes);
        return NULL;
    }
    return codes;
}

int commodity_aggregate_set_measure_condition_codes(CommodityAggregate *aggregate,
                                                    MeasureConditionCodeModelList condition_codes)
{
    if (aggregate == NULL) {
        measure_condition_code_model_list_free(&condition_codes);
        return 0;
    }

    measure_condition_code_model_list_free(&aggregate->measure_condition_codes);
    aggregate->measure_condition_codes = condition_codes;
    return 1;
}

char **commodity_aggregate_measure_condition_sids(const CommodityAggregate *aggregate,
                                                  size_t *sid_count)
{
    if (sid_count != NULL) {
        *sid_count = 0;
    }
    if (aggregate == NULL || sid_count == NULL || aggregate->measure_conditions.len == 0) {
        return NULL;
    }

    char **sids = calloc(aggregate->measure_conditions.len, sizeof(*sids));
    if (sids == NULL) {
        return NULL;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->measure_conditions.len; i++) {
        if (aggregate->measure_conditions.items[i].measure_condition_sid == NULL) {
            continue;
        }
        sids[count++] = aggregate->measure_conditions.items[i].measure_condition_sid;
    }

    if (count == 0) {
        free(sids);
        return NULL;
    }

    *sid_count = count;
    return sids;
}

int commodity_aggregate_set_measure_condition_components(CommodityAggregate *aggregate,
                                                         MeasureConditionComponentModelList components)
{
    if (aggregate == NULL) {
        measure_condition_component_model_list_free(&components);
        return 0;
    }

    measure_condition_component_model_list_free(&aggregate->measure_condition_components);
    aggregate->measure_condition_components = components;
    return 1;
}

size_t commodity_aggregate_measure_condition_count_for_measure(const CommodityAggregate *aggregate,
                                                               const char *measure_sid)
{
    if (aggregate == NULL || measure_sid == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->measure_conditions.len; i++) {
        if (aggregate->measure_conditions.items[i].measure_sid != NULL &&
            strcmp(aggregate->measure_conditions.items[i].measure_sid, measure_sid) == 0) {
            count++;
        }
    }
    return count;
}

size_t commodity_aggregate_measure_condition_component_count_for_condition(const CommodityAggregate *aggregate,
                                                                           const char *measure_condition_sid)
{
    if (aggregate == NULL || measure_condition_sid == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->measure_condition_components.len; i++) {
        if (aggregate->measure_condition_components.items[i].measure_condition_sid != NULL &&
            strcmp(aggregate->measure_condition_components.items[i].measure_condition_sid, measure_condition_sid) == 0) {
            count++;
        }
    }
    return count;
}

int commodity_aggregate_set_geographical_areas(CommodityAggregate *aggregate,
                                               GeographicalAreaModelList areas)
{
    if (aggregate == NULL) {
        geographical_area_model_list_free(&areas);
        return 0;
    }

    geographical_area_model_list_free(&aggregate->geographical_areas);
    aggregate->geographical_areas = areas;
    return 1;
}

int commodity_aggregate_set_excluded_geographical_areas(CommodityAggregate *aggregate,
                                                        MeasureExcludedGeographicalAreaModelList excluded)
{
    if (aggregate == NULL) {
        measure_excluded_geographical_area_model_list_free(&excluded);
        return 0;
    }

    measure_excluded_geographical_area_model_list_free(&aggregate->excluded_geographical_areas);
    aggregate->excluded_geographical_areas = excluded;
    return 1;
}

size_t commodity_aggregate_excluded_geographical_area_count_for_measure(const CommodityAggregate *aggregate,
                                                                        const char *measure_sid)
{
    if (aggregate == NULL || measure_sid == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->excluded_geographical_areas.len; i++) {
        if (aggregate->excluded_geographical_areas.items[i].measure_sid != NULL &&
            strcmp(aggregate->excluded_geographical_areas.items[i].measure_sid, measure_sid) == 0) {
            count++;
        }
    }
    return count;
}

char **commodity_aggregate_additional_code_sids(const CommodityAggregate *aggregate,
                                                size_t *sid_count)
{
    if (sid_count != NULL) {
        *sid_count = 0;
    }
    if (aggregate == NULL || sid_count == NULL || aggregate->applicable_measures.len == 0) {
        return NULL;
    }

    char **sids = calloc(aggregate->applicable_measures.len, sizeof(*sids));
    if (sids == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const char *sid = aggregate->applicable_measures.items[i].additional_code_sid;
        if (sid == NULL || *sid == '\0') {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *sid_count; j++) {
            if (strcmp(sids[j], sid) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            sids[*sid_count] = (char *)sid;
            (*sid_count)++;
        }
    }

    if (*sid_count == 0) {
        free(sids);
        return NULL;
    }
    return sids;
}

int commodity_aggregate_set_additional_codes(CommodityAggregate *aggregate,
                                             AdditionalCodeModelList additional_codes)
{
    if (aggregate == NULL) {
        additional_code_model_list_free(&additional_codes);
        return 0;
    }

    additional_code_model_list_free(&aggregate->additional_codes);
    aggregate->additional_codes = additional_codes;
    return 1;
}

int commodity_aggregate_set_commodity_footnotes(CommodityAggregate *aggregate,
                                                FootnoteList footnotes)
{
    if (aggregate == NULL) {
        footnote_list_free(&footnotes);
        return 0;
    }

    footnote_list_free(&aggregate->commodity_footnotes);
    aggregate->commodity_footnotes = footnotes;
    return 1;
}

int commodity_aggregate_set_measure_footnotes(CommodityAggregate *aggregate,
                                              FootnoteList footnotes)
{
    if (aggregate == NULL) {
        footnote_list_free(&footnotes);
        return 0;
    }

    footnote_list_free(&aggregate->measure_footnotes);
    aggregate->measure_footnotes = footnotes;
    return 1;
}

size_t commodity_aggregate_commodity_footnote_count(const CommodityAggregate *aggregate)
{
    return aggregate == NULL ? 0 : aggregate->commodity_footnotes.len;
}

size_t commodity_aggregate_footnote_count_for_measure(const CommodityAggregate *aggregate,
                                                       const char *measure_sid)
{
    if (aggregate == NULL || measure_sid == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->measure_footnotes.len; i++) {
        if (aggregate->measure_footnotes.items[i].measure_sid != NULL &&
            strcmp(aggregate->measure_footnotes.items[i].measure_sid, measure_sid) == 0) {
            count++;
        }
    }
    return count;
}

int commodity_aggregate_set_measure_legal_acts(CommodityAggregate *aggregate,
                                               MeasureLegalActModelList legal_acts)
{
    if (aggregate == NULL) {
        measure_legal_act_model_list_free(&legal_acts);
        return 0;
    }

    measure_legal_act_model_list_free(&aggregate->measure_legal_acts);
    aggregate->measure_legal_acts = legal_acts;
    return 1;
}

size_t commodity_aggregate_legal_act_count_for_measure(const CommodityAggregate *aggregate,
                                                       const char *measure_sid)
{
    if (aggregate == NULL || measure_sid == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->measure_legal_acts.len; i++) {
        if (aggregate->measure_legal_acts.items[i].measure_sid != NULL &&
            strcmp(aggregate->measure_legal_acts.items[i].measure_sid, measure_sid) == 0) {
            count++;
        }
    }
    return count;
}

char **commodity_aggregate_order_numbers(const CommodityAggregate *aggregate,
                                         size_t *order_number_count)
{
    if (order_number_count != NULL) {
        *order_number_count = 0;
    }
    if (aggregate == NULL || order_number_count == NULL || aggregate->applicable_measures.len == 0) {
        return NULL;
    }

    char **order_numbers = calloc(aggregate->applicable_measures.len, sizeof(*order_numbers));
    if (order_numbers == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const char *order_number = aggregate->applicable_measures.items[i].ordernumber;
        if (order_number == NULL || *order_number == '\0') {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *order_number_count; j++) {
            if (strcmp(order_numbers[j], order_number) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            order_numbers[*order_number_count] = (char *)order_number;
            (*order_number_count)++;
        }
    }

    if (*order_number_count == 0) {
        free(order_numbers);
        return NULL;
    }
    return order_numbers;
}

int commodity_aggregate_set_quota_order_numbers(CommodityAggregate *aggregate,
                                                QuotaOrderNumberModelList order_numbers)
{
    if (aggregate == NULL) {
        quota_order_number_model_list_free(&order_numbers);
        return 0;
    }

    quota_order_number_model_list_free(&aggregate->quota_order_numbers);
    aggregate->quota_order_numbers = order_numbers;
    return 1;
}

int commodity_aggregate_set_quota_definitions(CommodityAggregate *aggregate,
                                              QuotaDefinitionModelList definitions)
{
    if (aggregate == NULL) {
        quota_definition_model_list_free(&definitions);
        return 0;
    }

    quota_definition_model_list_free(&aggregate->quota_definitions);
    aggregate->quota_definitions = definitions;
    return 1;
}

int commodity_aggregate_set_quota_events(CommodityAggregate *aggregate,
                                         QuotaEventModelList events)
{
    if (aggregate == NULL) {
        quota_event_model_list_free(&events);
        return 0;
    }

    quota_event_model_list_free(&aggregate->quota_events);
    aggregate->quota_events = events;
    return 1;
}

char **commodity_aggregate_quota_definition_sids(const CommodityAggregate *aggregate,
                                                 size_t *sid_count)
{
    if (sid_count != NULL) {
        *sid_count = 0;
    }
    if (aggregate == NULL || sid_count == NULL || aggregate->quota_definitions.len == 0) {
        return NULL;
    }

    char **sids = calloc(aggregate->quota_definitions.len, sizeof(*sids));
    if (sids == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->quota_definitions.len; i++) {
        const char *sid = aggregate->quota_definitions.items[i].quota_definition_sid;
        if (sid == NULL || *sid == '\0') {
            continue;
        }

        int seen = 0;
        for (size_t j = 0; j < *sid_count; j++) {
            if (strcmp(sids[j], sid) == 0) {
                seen = 1;
                break;
            }
        }
        if (!seen) {
            sids[*sid_count] = (char *)sid;
            (*sid_count)++;
        }
    }

    if (*sid_count == 0) {
        free(sids);
        return NULL;
    }
    return sids;
}

int commodity_aggregate_set_quota_closed_and_transferred_events(
    CommodityAggregate *aggregate,
    QuotaClosedAndTransferredEventModelList events)
{
    if (aggregate == NULL) {
        quota_closed_and_transferred_event_model_list_free(&events);
        return 0;
    }

    quota_closed_and_transferred_event_model_list_free(&aggregate->quota_closed_and_transferred_events);
    aggregate->quota_closed_and_transferred_events = events;
    return 1;
}

size_t commodity_aggregate_quota_order_number_count(const CommodityAggregate *aggregate)
{
    return aggregate == NULL ? 0 : aggregate->quota_order_numbers.len;
}

size_t commodity_aggregate_quota_definition_count(const CommodityAggregate *aggregate)
{
    return aggregate == NULL ? 0 : aggregate->quota_definitions.len;
}

size_t commodity_aggregate_quota_event_count(const CommodityAggregate *aggregate)
{
    return aggregate == NULL ? 0 : aggregate->quota_events.len;
}

size_t commodity_aggregate_quota_closed_and_transferred_event_count(const CommodityAggregate *aggregate)
{
    return aggregate == NULL ? 0 : aggregate->quota_closed_and_transferred_events.len;
}

size_t commodity_aggregate_import_measure_count(const CommodityAggregate *aggregate)
{
    if (aggregate == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        if (measure_model_import(&aggregate->applicable_measures.items[i])) {
            count++;
        }
    }
    return count;
}

size_t commodity_aggregate_export_measure_count(const CommodityAggregate *aggregate)
{
    if (aggregate == NULL) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        if (measure_model_export(&aggregate->applicable_measures.items[i])) {
            count++;
        }
    }
    return count;
}

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

int commodity_aggregate_self_test(void)
{
    CommodityAggregate aggregate = {
        .commodity = {
            .goods_nomenclature_sid = copy_literal("123456"),
            .goods_nomenclature_item_id = copy_literal("8543400000"),
            .producline_suffix = copy_literal("80"),
            .number_indents = copy_literal("3"),
            .position = copy_literal("854340000080"),
            .depth = copy_literal("5"),
            .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
            .validity_end_date = NULL,
            .description = copy_literal("Electronic cigarettes"),
            .leaf = 1,
        },
    };

    GoodsNomenclatureTreeNode ancestor = {
        .goods_nomenclature_sid = copy_literal("654321"),
        .goods_nomenclature_item_id = copy_literal("8543000000"),
        .producline_suffix = copy_literal("10"),
        .number_indents = copy_literal("2"),
        .position = copy_literal("854300000010"),
        .depth = copy_literal("4"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .description = copy_literal("Ancestor"),
    };
    MeasureModelList measures = { 0 };
    MeasureModel import_measure = {
        .measure_sid = copy_literal("1"),
        .goods_nomenclature_sid = copy_literal("123456"),
        .measure_type_id = copy_literal("103"),
        .geographical_area_id = copy_literal("1011"),
        .additional_code_sid = copy_literal("9001"),
        .ordernumber = copy_literal("060004"),
        .trade_movement_code = copy_literal("0"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .effective_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
    MeasureModel both_measure = {
        .measure_sid = copy_literal("2"),
        .goods_nomenclature_sid = copy_literal("654321"),
        .measure_type_id = copy_literal("109"),
        .geographical_area_id = copy_literal("1011"),
        .trade_movement_code = copy_literal("2"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
        .effective_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
    MeasureComponentModelList components = { 0 };
    MeasureComponentModel component = {
        .measure_sid = copy_literal("1"),
        .duty_expression_id = copy_literal("01"),
        .duty_amount = copy_literal("0.0"),
        .measurement_unit_code = copy_literal("KGM"),
        .measurement_unit_qualifier_code = copy_literal("X"),
    };
    MeasurementUnitModelList measurement_units = { 0 };
    MeasurementUnitModel measurement_unit = {
        .measurement_unit_code = copy_literal("KGM"),
        .description = copy_literal("Kilogram"),
        .abbreviation = copy_literal("kg"),
    };
    MeasurementUnitQualifierModelList measurement_unit_qualifiers = { 0 };
    MeasurementUnitQualifierModel measurement_unit_qualifier = {
        .measurement_unit_qualifier_code = copy_literal("X"),
        .description = copy_literal("per hectolitre"),
    };
    MeasureConditionModelList conditions = { 0 };
    MeasureConditionModel condition = {
        .measure_condition_sid = copy_literal("10"),
        .measure_sid = copy_literal("1"),
        .condition_code = copy_literal("B"),
    };
    MeasureConditionComponentModelList condition_components = { 0 };
    MeasureConditionComponentModel condition_component = {
        .measure_condition_sid = copy_literal("10"),
        .duty_expression_id = copy_literal("01"),
        .duty_amount = copy_literal("1.0"),
    };
    GeographicalAreaModelList areas = { 0 };
    GeographicalAreaModel area = {
        .geographical_area_sid = copy_literal("100"),
        .geographical_area_id = copy_literal("1011"),
        .description = copy_literal("All countries"),
    };
    MeasureExcludedGeographicalAreaModelList excluded_areas = { 0 };
    MeasureExcludedGeographicalAreaModel excluded_area = {
        .measure_sid = copy_literal("1"),
        .geographical_area_sid = copy_literal("200"),
        .geographical_area_id = copy_literal("AD"),
    };
    AdditionalCodeModelList additional_codes = { 0 };
    AdditionalCodeModel additional_code = {
        .additional_code_sid = copy_literal("9001"),
        .additional_code_type_id = copy_literal("8"),
        .additional_code = copy_literal("123"),
        .description = copy_literal("Additional duty"),
    };
    ImportTradeSummaryModel import_trade_summary = {
        .id = copy_literal("summary-id"),
        .basic_third_country_duty = copy_literal("0 %"),
    };
    GuideModelList chapter_guides = { 0 };
    GuideModel chapter_guide = {
        .id = copy_literal("12"),
        .title = copy_literal("Chapter guide"),
        .url = copy_literal("https://example.test/guide"),
    };
    FootnoteList commodity_footnotes = { 0 };
    Footnote commodity_footnote = {
        .measure_sid = NULL,
        .id = copy_literal("TN701"),
        .code = copy_literal("TN701"),
        .description = copy_literal("Commodity footnote text"),
    };
    FootnoteList footnotes = { 0 };
    Footnote footnote = {
        .measure_sid = copy_literal("1"),
        .id = copy_literal("CD123"),
        .code = copy_literal("CD123"),
        .description = copy_literal("Footnote text"),
    };
    MeasureLegalActModelList legal_acts = { 0 };
    MeasureLegalActModel legal_act = {
        .measure_sid = copy_literal("1"),
        .measure_type_id = copy_literal("103"),
        .regulation_id = copy_literal("R230001"),
        .role = copy_literal("1"),
        .source = copy_literal("generating"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
    QuotaOrderNumberModelList quota_order_numbers = { 0 };
    QuotaOrderNumberModel quota_order_number = {
        .quota_order_number_sid = copy_literal("777"),
        .quota_order_number_id = copy_literal("060004"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
    QuotaDefinitionModelList quota_definitions = { 0 };
    QuotaDefinitionModel quota_definition = {
        .quota_definition_sid = copy_literal("888"),
        .quota_order_number_id = copy_literal("060004"),
        .quota_order_number_sid = copy_literal("777"),
        .initial_volume = copy_literal("1000"),
        .balance = copy_literal("900"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };
    QuotaEventModelList quota_events = { 0 };
    QuotaEventModel quota_event = {
        .quota_definition_sid = copy_literal("888"),
        .occurrence_timestamp = copy_literal("2024-01-01T00:00:00.000Z"),
        .event_type = copy_literal("balance"),
    };

    size_t sid_count = 0;
    char **sids = NULL;
    size_t measure_sid_count = 0;
    char **measure_sids = NULL;
    size_t measurement_unit_code_count = 0;
    char **measurement_unit_codes = NULL;
    size_t measurement_unit_qualifier_code_count = 0;
    char **measurement_unit_qualifier_codes = NULL;
    size_t additional_code_sid_count = 0;
    char **additional_code_sids = NULL;
    size_t order_number_count = 0;
    char **order_numbers = NULL;
    int ok = goods_nomenclature_tree_node_list_append(&aggregate.ancestors, ancestor) &&
             measure_model_list_append(&measures, import_measure) &&
             measure_model_list_append(&measures, both_measure) &&
             measure_component_model_list_append(&components, component) &&
             measurement_unit_model_list_append(&measurement_units, measurement_unit) &&
             measurement_unit_qualifier_model_list_append(&measurement_unit_qualifiers, measurement_unit_qualifier) &&
             measure_condition_model_list_append(&conditions, condition) &&
             measure_condition_component_model_list_append(&condition_components, condition_component) &&
             geographical_area_model_list_append(&areas, area) &&
             measure_excluded_geographical_area_model_list_append(&excluded_areas, excluded_area) &&
             additional_code_model_list_append(&additional_codes, additional_code) &&
             guide_model_list_append(&chapter_guides, chapter_guide) &&
             footnote_list_append(&commodity_footnotes, commodity_footnote) &&
             footnote_list_append(&footnotes, footnote) &&
             measure_legal_act_model_list_append(&legal_acts, legal_act) &&
             quota_order_number_model_list_append(&quota_order_numbers, quota_order_number) &&
             quota_definition_model_list_append(&quota_definitions, quota_definition) &&
             quota_event_model_list_append(&quota_events, quota_event) &&
             commodity_aggregate_set_applicable_measures(&aggregate, measures) &&
             commodity_aggregate_set_measure_components(&aggregate, components) &&
             commodity_aggregate_set_measurement_units(&aggregate, measurement_units) &&
             commodity_aggregate_set_measurement_unit_qualifiers(&aggregate, measurement_unit_qualifiers) &&
             commodity_aggregate_set_measure_conditions(&aggregate, conditions) &&
             commodity_aggregate_set_measure_condition_components(&aggregate, condition_components) &&
             commodity_aggregate_set_geographical_areas(&aggregate, areas) &&
             commodity_aggregate_set_excluded_geographical_areas(&aggregate, excluded_areas) &&
             commodity_aggregate_set_additional_codes(&aggregate, additional_codes) &&
             commodity_aggregate_set_chapter_guides(&aggregate, chapter_guides) &&
             commodity_aggregate_set_import_trade_summary(&aggregate, import_trade_summary) &&
             commodity_aggregate_set_commodity_footnotes(&aggregate, commodity_footnotes) &&
             commodity_aggregate_set_measure_footnotes(&aggregate, footnotes) &&
             commodity_aggregate_set_measure_legal_acts(&aggregate, legal_acts) &&
             commodity_aggregate_set_quota_order_numbers(&aggregate, quota_order_numbers) &&
             commodity_aggregate_set_quota_definitions(&aggregate, quota_definitions) &&
             commodity_aggregate_set_quota_events(&aggregate, quota_events) &&
             strcmp(aggregate.commodity.goods_nomenclature_sid, "123456") == 0 &&
             commodity_aggregate_import_measure_count(&aggregate) == 2 &&
             commodity_aggregate_export_measure_count(&aggregate) == 1 &&
             commodity_aggregate_measure_component_count_for_measure(&aggregate, "1") == 1 &&
             commodity_aggregate_measure_condition_count_for_measure(&aggregate, "1") == 1 &&
             commodity_aggregate_measure_condition_component_count_for_condition(&aggregate, "10") == 1 &&
             commodity_aggregate_excluded_geographical_area_count_for_measure(&aggregate, "1") == 1 &&
             commodity_aggregate_commodity_footnote_count(&aggregate) == 1 &&
             commodity_aggregate_footnote_count_for_measure(&aggregate, "1") == 1 &&
             commodity_aggregate_legal_act_count_for_measure(&aggregate, "1") == 1 &&
             commodity_aggregate_quota_order_number_count(&aggregate) == 1 &&
             commodity_aggregate_quota_definition_count(&aggregate) == 1 &&
             commodity_aggregate_quota_event_count(&aggregate) == 1 &&
             aggregate.import_trade_summary.id != NULL &&
             aggregate.chapter_guides.len == 1 &&
             aggregate.measurement_units.len == 1 &&
             aggregate.measurement_unit_qualifiers.len == 1 &&
             aggregate.additional_codes.len == 1;

    if (ok) {
        sids = commodity_aggregate_goods_nomenclature_sids(&aggregate, &sid_count);
        ok = sids != NULL &&
             sid_count == 2 &&
             strcmp(sids[0], "654321") == 0 &&
             strcmp(sids[1], "123456") == 0;
    }

    if (ok) {
        measure_sids = commodity_aggregate_measure_sids(&aggregate, &measure_sid_count);
        ok = measure_sids != NULL &&
             measure_sid_count == 2 &&
             strcmp(measure_sids[0], "1") == 0 &&
             strcmp(measure_sids[1], "2") == 0;
    }

    if (ok) {
        size_t condition_sid_count = 0;
        char **condition_sids = commodity_aggregate_measure_condition_sids(&aggregate, &condition_sid_count);
        ok = condition_sids != NULL &&
             condition_sid_count == 1 &&
             strcmp(condition_sids[0], "10") == 0;
        free(condition_sids);
    }

    if (ok) {
        measurement_unit_codes = commodity_aggregate_measurement_unit_codes(&aggregate, &measurement_unit_code_count);
        ok = measurement_unit_codes != NULL &&
             measurement_unit_code_count == 1 &&
             strcmp(measurement_unit_codes[0], "KGM") == 0;
    }

    if (ok) {
        measurement_unit_qualifier_codes = commodity_aggregate_measurement_unit_qualifier_codes(&aggregate, &measurement_unit_qualifier_code_count);
        ok = measurement_unit_qualifier_codes != NULL &&
             measurement_unit_qualifier_code_count == 1 &&
             strcmp(measurement_unit_qualifier_codes[0], "X") == 0;
    }

    if (ok) {
        additional_code_sids = commodity_aggregate_additional_code_sids(&aggregate, &additional_code_sid_count);
        ok = additional_code_sids != NULL &&
             additional_code_sid_count == 1 &&
             strcmp(additional_code_sids[0], "9001") == 0;
    }

    if (ok) {
        order_numbers = commodity_aggregate_order_numbers(&aggregate, &order_number_count);
        ok = order_numbers != NULL &&
             order_number_count == 1 &&
             strcmp(order_numbers[0], "060004") == 0;
    }

    free(sids);
    free(measure_sids);
    free(measurement_unit_codes);
    free(measurement_unit_qualifier_codes);
    free(additional_code_sids);
    free(order_numbers);
    commodity_aggregate_free(&aggregate);
    return ok;
}
