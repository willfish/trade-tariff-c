#ifndef TRADE_TARIFF_C_COMMODITY_AGGREGATE_H
#define TRADE_TARIFF_C_COMMODITY_AGGREGATE_H

#include "domain/goods_nomenclature_tree.h"
#include "models/additional_code_model.h"
#include "models/appendix_5a_model.h"
#include "models/classification_model.h"
#include "models/footnote.h"
#include "models/geographical_area_model.h"
#include "models/guide_model.h"
#include "models/import_trade_summary_model.h"
#include "models/measure_component_model.h"
#include "models/measure_condition_code_model.h"
#include "models/measure_condition_component_model.h"
#include "models/measure_condition_model.h"
#include "models/measure_condition_permutation_model.h"
#include "models/measure_excluded_geographical_area_model.h"
#include "models/measure_legal_act_model.h"
#include "models/measure_model.h"
#include "models/measure_type_model.h"
#include "models/measurement_unit_model.h"
#include "models/measurement_unit_qualifier_model.h"
#include "models/quota_closed_and_transferred_event_model.h"
#include "models/quota_definition_model.h"
#include "models/quota_event_model.h"
#include "models/quota_order_number_model.h"

#include <stddef.h>

typedef struct {
    GoodsNomenclatureTreeNode commodity;
    GoodsNomenclatureTreeNodeList ancestors;
    ClassificationModel classification;
    GuideModelList chapter_guides;
    ImportTradeSummaryModel import_trade_summary;
    MeasureModelList applicable_measures;
    MeasureTypeModelList measure_types;
    MeasureComponentModelList measure_components;
    MeasureComponentModelList resolved_measure_components;
    MeasurementUnitModelList measurement_units;
    MeasurementUnitQualifierModelList measurement_unit_qualifiers;
    MeasureConditionModelList measure_conditions;
    MeasureConditionPermutationGroupModelList measure_condition_permutation_groups;
    Appendix5aModelList appendix_5as;
    MeasureConditionCodeModelList measure_condition_codes;
    MeasureConditionComponentModelList measure_condition_components;
    GeographicalAreaModelList geographical_areas;
    MeasureExcludedGeographicalAreaModelList excluded_geographical_areas;
    AdditionalCodeModelList additional_codes;
    FootnoteList commodity_footnotes;
    FootnoteList measure_footnotes;
    MeasureLegalActModelList measure_legal_acts;
    QuotaOrderNumberModelList quota_order_numbers;
    QuotaDefinitionModelList quota_definitions;
    QuotaEventModelList quota_events;
    QuotaClosedAndTransferredEventModelList quota_closed_and_transferred_events;
} CommodityAggregate;

void commodity_aggregate_free(CommodityAggregate *aggregate);
char **commodity_aggregate_goods_nomenclature_sids(const CommodityAggregate *aggregate,
                                                   size_t *sid_count);
int commodity_aggregate_set_applicable_measures(CommodityAggregate *aggregate,
                                                MeasureModelList measures);
int commodity_aggregate_set_classification(CommodityAggregate *aggregate,
                                           ClassificationModel classification);
int commodity_aggregate_set_chapter_guides(CommodityAggregate *aggregate,
                                           GuideModelList guides);
int commodity_aggregate_set_import_trade_summary(CommodityAggregate *aggregate,
                                                 ImportTradeSummaryModel import_trade_summary);
char **commodity_aggregate_measure_sids(const CommodityAggregate *aggregate,
                                        size_t *sid_count);
char **commodity_aggregate_measure_type_ids(const CommodityAggregate *aggregate,
                                            size_t *id_count);
int commodity_aggregate_set_measure_types(CommodityAggregate *aggregate,
                                          MeasureTypeModelList measure_types);
int commodity_aggregate_set_measure_components(CommodityAggregate *aggregate,
                                               MeasureComponentModelList components);
int commodity_aggregate_set_resolved_measure_components(CommodityAggregate *aggregate,
                                                        MeasureComponentModelList components);
size_t commodity_aggregate_measure_component_count_for_measure(const CommodityAggregate *aggregate,
                                                               const char *measure_sid);
char **commodity_aggregate_measurement_unit_codes(const CommodityAggregate *aggregate,
                                                  size_t *code_count);
char **commodity_aggregate_measurement_unit_qualifier_codes(const CommodityAggregate *aggregate,
                                                            size_t *code_count);
int commodity_aggregate_set_measurement_units(CommodityAggregate *aggregate,
                                              MeasurementUnitModelList units);
int commodity_aggregate_set_measurement_unit_qualifiers(CommodityAggregate *aggregate,
                                                        MeasurementUnitQualifierModelList qualifiers);
int commodity_aggregate_set_measure_conditions(CommodityAggregate *aggregate,
                                               MeasureConditionModelList conditions);
int commodity_aggregate_set_measure_condition_permutation_groups(
    CommodityAggregate *aggregate,
    MeasureConditionPermutationGroupModelList groups);
char **commodity_aggregate_appendix_5a_document_codes(const CommodityAggregate *aggregate,
                                                      size_t *document_code_count);
int commodity_aggregate_set_appendix_5as(CommodityAggregate *aggregate,
                                         Appendix5aModelList appendix_5as);
char **commodity_aggregate_measure_condition_codes(const CommodityAggregate *aggregate,
                                                   size_t *code_count);
int commodity_aggregate_set_measure_condition_codes(CommodityAggregate *aggregate,
                                                    MeasureConditionCodeModelList condition_codes);
char **commodity_aggregate_measure_condition_sids(const CommodityAggregate *aggregate,
                                                  size_t *sid_count);
int commodity_aggregate_set_measure_condition_components(CommodityAggregate *aggregate,
                                                         MeasureConditionComponentModelList components);
size_t commodity_aggregate_measure_condition_count_for_measure(const CommodityAggregate *aggregate,
                                                               const char *measure_sid);
size_t commodity_aggregate_measure_condition_component_count_for_condition(const CommodityAggregate *aggregate,
                                                                           const char *measure_condition_sid);
int commodity_aggregate_set_geographical_areas(CommodityAggregate *aggregate,
                                               GeographicalAreaModelList areas);
int commodity_aggregate_set_excluded_geographical_areas(CommodityAggregate *aggregate,
                                                        MeasureExcludedGeographicalAreaModelList excluded);
size_t commodity_aggregate_excluded_geographical_area_count_for_measure(const CommodityAggregate *aggregate,
                                                                        const char *measure_sid);
char **commodity_aggregate_additional_code_sids(const CommodityAggregate *aggregate,
                                                size_t *sid_count);
int commodity_aggregate_set_additional_codes(CommodityAggregate *aggregate,
                                             AdditionalCodeModelList additional_codes);
int commodity_aggregate_set_commodity_footnotes(CommodityAggregate *aggregate,
                                                FootnoteList footnotes);
int commodity_aggregate_set_measure_footnotes(CommodityAggregate *aggregate,
                                              FootnoteList footnotes);
size_t commodity_aggregate_commodity_footnote_count(const CommodityAggregate *aggregate);
size_t commodity_aggregate_footnote_count_for_measure(const CommodityAggregate *aggregate,
                                                       const char *measure_sid);
int commodity_aggregate_set_measure_legal_acts(CommodityAggregate *aggregate,
                                               MeasureLegalActModelList legal_acts);
size_t commodity_aggregate_legal_act_count_for_measure(const CommodityAggregate *aggregate,
                                                       const char *measure_sid);
char **commodity_aggregate_order_numbers(const CommodityAggregate *aggregate,
                                         size_t *order_number_count);
int commodity_aggregate_set_quota_order_numbers(CommodityAggregate *aggregate,
                                                QuotaOrderNumberModelList order_numbers);
int commodity_aggregate_set_quota_definitions(CommodityAggregate *aggregate,
                                              QuotaDefinitionModelList definitions);
char **commodity_aggregate_quota_definition_sids(const CommodityAggregate *aggregate,
                                                 size_t *sid_count);
int commodity_aggregate_set_quota_events(CommodityAggregate *aggregate,
                                         QuotaEventModelList events);
int commodity_aggregate_set_quota_closed_and_transferred_events(
    CommodityAggregate *aggregate,
    QuotaClosedAndTransferredEventModelList events);
size_t commodity_aggregate_quota_order_number_count(const CommodityAggregate *aggregate);
size_t commodity_aggregate_quota_definition_count(const CommodityAggregate *aggregate);
size_t commodity_aggregate_quota_event_count(const CommodityAggregate *aggregate);
size_t commodity_aggregate_quota_closed_and_transferred_event_count(const CommodityAggregate *aggregate);
size_t commodity_aggregate_import_measure_count(const CommodityAggregate *aggregate);
size_t commodity_aggregate_export_measure_count(const CommodityAggregate *aggregate);
int commodity_aggregate_self_test(void);

#endif
