#include "loaders/commodity_loader.h"

#include "loaders/commodity_load_plan.h"
#include "loaders/measure_graph_loader.h"
#include "presenters/import_trade_summary_presenter.h"
#include "repositories/classification_repository.h"
#include "repositories/commodity_footnote_repository.h"
#include "repositories/goods_nomenclature_tree_repository.h"
#include "repositories/guide_repository.h"

static GoodsNomenclatureTreeRepositoryQuery tree_query_from_context(const CommodityRequestContext *context)
{
    return (GoodsNomenclatureTreeRepositoryQuery){
        .schema = context->schema,
        .actual_date = context->actual_date,
        .commodity_id = context->commodity_id,
    };
}

int commodity_loader_load_with_plan(const CommodityRequestContext *context,
                                    const CommodityIncludePlan *plan,
                                    CommodityAggregate *aggregate)
{
    if (context == NULL ||
        context->schema == NULL ||
        context->commodity_id == NULL ||
        plan == NULL ||
        aggregate == NULL) {
        return 0;
    }

    *aggregate = (CommodityAggregate){ 0 };
    CommodityLoadPlan load_plan = commodity_load_plan_from_include_plan(plan);
    GoodsNomenclatureTreeRepositoryQuery tree_query = tree_query_from_context(context);

    ClassificationModel classification = { 0 };
    GuideModelList chapter_guides = { 0 };
    FootnoteList commodity_footnotes = { 0 };
    ImportTradeSummaryModel import_trade_summary = { 0 };
    if (!goods_nomenclature_tree_repository_fetch_origin(&tree_query, &aggregate->commodity)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.ancestors &&
        !goods_nomenclature_tree_repository_fetch_ancestors(&tree_query, &aggregate->ancestors)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.classification &&
        (!classification_repository_fetch_for_commodity(context, &classification) ||
         !commodity_aggregate_set_classification(aggregate, classification))) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.chapter_guides &&
        (!guide_repository_fetch_for_goods_nomenclature_sids(context,
                                                             (const char *const[]){
                                                                 aggregate->classification.has_chapter ? aggregate->classification.chapter.goods_nomenclature_sid : NULL,
                                                             },
                                                             1,
                                                             &chapter_guides) ||
         !commodity_aggregate_set_chapter_guides(aggregate, chapter_guides))) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.commodity_footnotes &&
        (!commodity_footnote_repository_fetch_for_goods_nomenclature_sids(context,
                                                                          (const char *const[]){
                                                                              aggregate->commodity.goods_nomenclature_sid,
                                                                              aggregate->classification.has_heading ? aggregate->classification.heading.goods_nomenclature_sid : NULL,
                                                                          },
                                                                          2,
                                                                          &commodity_footnotes) ||
         !commodity_aggregate_set_commodity_footnotes(aggregate, commodity_footnotes))) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.applicable_measures &&
        !measure_graph_loader_load_applicable_measures(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_components &&
        !measure_graph_loader_load_measure_components(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_types &&
        !measure_graph_loader_load_measure_types(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.resolved_measure_components &&
        !measure_graph_loader_load_resolved_measure_components(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measurement_units &&
        !measure_graph_loader_load_measurement_units(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measurement_unit_qualifiers &&
        !measure_graph_loader_load_measurement_unit_qualifiers(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.import_trade_summary &&
        (!import_trade_summary_presenter_build(&aggregate->applicable_measures,
                                               &aggregate->measure_components,
                                               &import_trade_summary) ||
         !commodity_aggregate_set_import_trade_summary(aggregate, import_trade_summary))) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_conditions &&
        !measure_graph_loader_load_measure_conditions(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_condition_permutation_groups &&
        !measure_graph_loader_load_measure_condition_permutation_groups(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.appendix_5as &&
        !measure_graph_loader_load_appendix_5as(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_condition_codes &&
        !measure_graph_loader_load_measure_condition_codes(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_condition_components &&
        !measure_graph_loader_load_measure_condition_components(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.geographical_areas &&
        !measure_graph_loader_load_geographical_areas(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.excluded_geographical_areas &&
        !measure_graph_loader_load_excluded_geographical_areas(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.additional_codes &&
        !measure_graph_loader_load_additional_codes(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_footnotes &&
        !measure_graph_loader_load_measure_footnotes(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.measure_legal_acts &&
        !measure_graph_loader_load_measure_legal_acts(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.quota_order_numbers &&
        !measure_graph_loader_load_quota_order_numbers(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.quota_definitions &&
        !measure_graph_loader_load_quota_definitions(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.quota_events &&
        !measure_graph_loader_load_quota_events(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    if (load_plan.quota_closed_and_transferred_events &&
        !measure_graph_loader_load_quota_closed_and_transferred_events(context, aggregate)) {
        commodity_aggregate_free(aggregate);
        return 0;
    }

    return 1;
}

int commodity_loader_load(const CommodityRequestContext *context,
                          CommodityAggregate *aggregate)
{
    CommodityIncludePlan plan = commodity_include_plan_default();
    return commodity_loader_load_with_plan(context, &plan, aggregate);
}

int commodity_loader_self_test(void)
{
    CommodityAggregate aggregate = { 0 };
    CommodityRequestContext missing_schema = {
        .schema = NULL,
        .actual_date = NULL,
        .commodity_id = "8543400000",
    };
    CommodityRequestContext missing_commodity = {
        .schema = "uk",
        .actual_date = NULL,
        .commodity_id = NULL,
    };

    return commodity_loader_load(NULL, &aggregate) == 0 &&
           commodity_loader_load(&missing_schema, &aggregate) == 0 &&
           commodity_loader_load(&missing_commodity, &aggregate) == 0 &&
           commodity_loader_load(&(CommodityRequestContext){
               .schema = "uk",
               .actual_date = NULL,
               .commodity_id = "8543400000",
           }, NULL) == 0 &&
           commodity_loader_load_with_plan(&(CommodityRequestContext){
               .schema = "uk",
               .actual_date = NULL,
               .commodity_id = "8543400000",
           }, NULL, &aggregate) == 0;
}
