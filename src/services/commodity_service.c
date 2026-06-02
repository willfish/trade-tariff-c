#include "services/commodity_service.h"

#include "formatters/description_formatter.h"
#include "json.h"
#include "json_builder.h"
#include "domain/commodity_aggregate.h"
#include "loaders/commodity_load_plan.h"
#include "loaders/commodity_loader.h"
#include "loaders/measure_graph_loader.h"
#include "domain/jsonapi_include_index.h"
#include "models/additional_code_model.h"
#include "models/appendix_5a_model.h"
#include "models/classification_model.h"
#include "models/duty_expression_model.h"
#include "models/geographical_area_model.h"
#include "models/goods_nomenclature_description.h"
#include "models/guide_model.h"
#include "models/import_trade_summary_model.h"
#include "models/measure_component_model.h"
#include "models/measure_condition_code_model.h"
#include "models/measure_condition_component_model.h"
#include "models/measure_condition_model.h"
#include "models/measure_condition_permutation_model.h"
#include "models/measure_excluded_geographical_area_model.h"
#include "models/measure_legal_act_model.h"
#include "models/measure_type_model.h"
#include "models/measurement_unit_model.h"
#include "models/measurement_unit_qualifier_model.h"
#include "models/quota_definition_model.h"
#include "models/quota_event_model.h"
#include "models/quota_order_number_model.h"
#include "presenters/additional_code_presenter.h"
#include "presenters/certificate_presenter.h"
#include "presenters/commodity_presenter.h"
#include "presenters/duty_expression_presenter.h"
#include "presenters/duty_expression_description_presenter.h"
#include "presenters/footnote_presenter.h"
#include "presenters/geographical_area_presenter.h"
#include "presenters/goods_nomenclature_description_presenter.h"
#include "presenters/guide_presenter.h"
#include "presenters/import_trade_summary_presenter.h"
#include "presenters/measure_component_presenter.h"
#include "presenters/measure_condition_code_presenter.h"
#include "presenters/measure_condition_component_presenter.h"
#include "presenters/measure_condition_presenter.h"
#include "presenters/measure_condition_permutation_presenter.h"
#include "presenters/measure_duty_expression_presenter.h"
#include "presenters/measure_legal_act_presenter.h"
#include "presenters/measure_presenter.h"
#include "presenters/measure_type_presenter.h"
#include "presenters/measurement_unit_presenter.h"
#include "presenters/measurement_unit_qualifier_presenter.h"
#include "presenters/monetary_unit_presenter.h"
#include "presenters/oplog_presenters.h"
#include "presenters/preference_code_presenter.h"
#include "presenters/quota_closed_and_transferred_event_presenter.h"
#include "presenters/quota_definition_presenter.h"
#include "presenters/quota_order_number_presenter.h"
#include "presenters/regulation_presenter.h"
#include "request/commodity_include_plan.h"
#include "repositories/commodity_repository.h"
#include "repositories/additional_code_repository.h"
#include "repositories/appendix_5a_repository.h"
#include "repositories/classification_repository.h"
#include "repositories/commodity_footnote_repository.h"
#include "repositories/geographical_area_repository.h"
#include "repositories/goods_nomenclature_tree_repository.h"
#include "repositories/guide_repository.h"
#include "repositories/measure_component_repository.h"
#include "repositories/measure_condition_code_repository.h"
#include "repositories/measure_condition_component_repository.h"
#include "repositories/measure_condition_repository.h"
#include "repositories/measure_excluded_geographical_area_repository.h"
#include "repositories/measure_footnote_repository.h"
#include "repositories/measure_legal_act_repository.h"
#include "repositories/measure_repository.h"
#include "repositories/measure_type_repository.h"
#include "repositories/measurement_unit_repository.h"
#include "repositories/measurement_unit_qualifier_repository.h"
#include "repositories/oplog_table_repository.h"
#include "repositories/quota_definition_repository.h"
#include "repositories/quota_event_repository.h"
#include "repositories/quota_order_number_repository.h"
#include "renderers/commodity_tree_slice_renderer.h"
#include "renderers/additional_code_jsonapi_renderer.h"
#include "renderers/classification_jsonapi_renderer.h"
#include "renderers/duty_expression_jsonapi_renderer.h"
#include "renderers/footnote_jsonapi_renderer.h"
#include "renderers/geographical_area_jsonapi_renderer.h"
#include "renderers/guide_jsonapi_renderer.h"
#include "renderers/import_trade_summary_jsonapi_renderer.h"
#include "renderers/jsonapi_relationship.h"
#include "renderers/measure_component_jsonapi_renderer.h"
#include "renderers/measure_condition_code_jsonapi_renderer.h"
#include "renderers/measure_condition_component_jsonapi_renderer.h"
#include "renderers/measure_condition_jsonapi_renderer.h"
#include "renderers/measure_condition_permutation_jsonapi_renderer.h"
#include "renderers/measure_jsonapi_renderer.h"
#include "renderers/measure_legal_act_jsonapi_renderer.h"
#include "renderers/measure_type_jsonapi_renderer.h"
#include "renderers/measurement_unit_jsonapi_renderer.h"
#include "renderers/measurement_unit_qualifier_jsonapi_renderer.h"
#include "renderers/quota_definition_jsonapi_renderer.h"
#include "renderers/quota_order_number_jsonapi_renderer.h"

#include <stdlib.h>

static HttpResponse render_typed_commodity_json(const CommodityRequest *request)
{
    CommodityRequestContext context = {
        .schema = request->service,
        .actual_date = request->actual_date,
        .commodity_id = request->commodity_id,
        .meursing_additional_code_id = request->meursing_additional_code_id,
    };
    CommodityAggregate aggregate = { 0 };
    if (!commodity_loader_load_with_plan(&context, &request->include_plan, &aggregate)) {
        return (HttpResponse){ HTTP_NOT_FOUND, json_error("Not Found", "Commodity was not found") };
    }

    char *body = commodity_tree_slice_render_jsonapi_with_plan(&aggregate,
                                                               request->service,
                                                               &request->include_plan);
    commodity_aggregate_free(&aggregate);
    if (body == NULL) {
        return (HttpResponse){ HTTP_INTERNAL_SERVER_ERROR, json_internal_error() };
    }

    return (HttpResponse){ HTTP_OK, body };
}

HttpResponse commodity_service_show_json(const CommodityRequest *request)
{
    return render_typed_commodity_json(request);
}

int commodity_service_self_test(void)
{
    return commodity_request_self_test() &&
           json_builder_self_test() &&
           description_formatter_self_test() &&
           commodity_aggregate_self_test() &&
           jsonapi_include_index_self_test() &&
           jsonapi_relationship_self_test() &&
           additional_code_model_self_test() &&
           appendix_5a_model_self_test() &&
           classification_model_self_test() &&
           duty_expression_model_self_test() &&
           footnote_self_test() &&
           goods_nomenclature_description_self_test() &&
           guide_model_self_test() &&
           import_trade_summary_model_self_test() &&
           goods_nomenclature_tree_self_test() &&
           geographical_area_model_self_test() &&
           measure_model_self_test() &&
           measure_component_model_self_test() &&
           measure_condition_code_model_self_test() &&
           measure_condition_model_self_test() &&
           measure_condition_component_model_self_test() &&
           measure_condition_permutation_model_self_test() &&
           measure_excluded_geographical_area_model_self_test() &&
           measure_legal_act_model_self_test() &&
           measure_type_model_self_test() &&
           measurement_unit_model_self_test() &&
           measurement_unit_qualifier_model_self_test() &&
           quota_order_number_model_self_test() &&
           quota_definition_model_self_test() &&
           quota_event_model_self_test() &&
           commodity_include_plan_self_test() &&
           oplog_table_repository_self_test() &&
           commodity_repository_self_test() &&
           additional_code_repository_self_test() &&
           appendix_5a_repository_self_test() &&
           classification_repository_self_test() &&
           commodity_footnote_repository_self_test() &&
           goods_nomenclature_tree_repository_self_test() &&
           guide_repository_self_test() &&
           geographical_area_repository_self_test() &&
           measure_repository_self_test() &&
           measure_component_repository_self_test() &&
           measure_condition_code_repository_self_test() &&
           measure_condition_repository_self_test() &&
           measure_condition_component_repository_self_test() &&
           measure_excluded_geographical_area_repository_self_test() &&
           measure_footnote_repository_self_test() &&
           measure_legal_act_repository_self_test() &&
           measure_type_repository_self_test() &&
           measurement_unit_repository_self_test() &&
           measurement_unit_qualifier_repository_self_test() &&
           quota_order_number_repository_self_test() &&
           quota_definition_repository_self_test() &&
           quota_event_repository_self_test() &&
           commodity_load_plan_self_test() &&
           commodity_loader_self_test() &&
           measure_graph_loader_self_test() &&
           additional_code_presenter_self_test() &&
           certificate_presenter_self_test() &&
           commodity_presenter_self_test() &&
           duty_expression_presenter_self_test() &&
           duty_expression_description_presenter_self_test() &&
           footnote_presenter_self_test() &&
           geographical_area_presenter_self_test() &&
           goods_nomenclature_description_presenter_self_test() &&
           guide_presenter_self_test() &&
           import_trade_summary_presenter_self_test() &&
           measure_component_presenter_self_test() &&
           measure_condition_code_presenter_self_test() &&
           measure_condition_component_presenter_self_test() &&
           measure_condition_presenter_self_test() &&
           measure_condition_permutation_presenter_self_test() &&
           measure_duty_expression_presenter_self_test() &&
           measure_legal_act_presenter_self_test() &&
           measure_presenter_self_test() &&
           measure_type_presenter_self_test() &&
           measurement_unit_presenter_self_test() &&
           measurement_unit_qualifier_presenter_self_test() &&
           monetary_unit_presenter_self_test() &&
           preference_code_presenter_self_test() &&
           oplog_presenters_self_test() &&
           quota_closed_and_transferred_event_presenter_self_test() &&
           quota_definition_presenter_self_test() &&
           quota_order_number_presenter_self_test() &&
           regulation_presenter_self_test() &&
           additional_code_jsonapi_renderer_self_test() &&
           classification_jsonapi_renderer_self_test() &&
           duty_expression_jsonapi_renderer_self_test() &&
           footnote_jsonapi_renderer_self_test() &&
           geographical_area_jsonapi_renderer_self_test() &&
           guide_jsonapi_renderer_self_test() &&
           import_trade_summary_jsonapi_renderer_self_test() &&
           measure_component_jsonapi_renderer_self_test() &&
           measure_condition_code_jsonapi_renderer_self_test() &&
           measure_condition_jsonapi_renderer_self_test() &&
           measure_condition_component_jsonapi_renderer_self_test() &&
           measure_condition_permutation_jsonapi_renderer_self_test() &&
           measure_legal_act_jsonapi_renderer_self_test() &&
           measure_type_jsonapi_renderer_self_test() &&
           measurement_unit_jsonapi_renderer_self_test() &&
           measurement_unit_qualifier_jsonapi_renderer_self_test() &&
           quota_order_number_jsonapi_renderer_self_test() &&
           quota_definition_jsonapi_renderer_self_test() &&
           measure_jsonapi_renderer_self_test() &&
           commodity_tree_slice_renderer_self_test();
}
