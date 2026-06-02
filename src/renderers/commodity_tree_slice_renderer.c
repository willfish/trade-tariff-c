#include "renderers/commodity_tree_slice_renderer.h"

#include "domain/goods_nomenclature_tree.h"
#include "domain/jsonapi_include_index.h"
#include "models/goods_nomenclature_description.h"
#include "presenters/additional_code_presenter.h"
#include "presenters/classification_presenter.h"
#include "presenters/commodity_presenter.h"
#include "presenters/duty_expression_presenter.h"
#include "presenters/footnote_presenter.h"
#include "presenters/geographical_area_presenter.h"
#include "presenters/goods_nomenclature_description_presenter.h"
#include "presenters/guide_presenter.h"
#include "presenters/import_trade_summary_presenter.h"
#include "presenters/measure_component_presenter.h"
#include "presenters/measure_condition_code_presenter.h"
#include "presenters/measure_condition_component_presenter.h"
#include "presenters/measure_condition_permutation_presenter.h"
#include "presenters/measure_condition_presenter.h"
#include "presenters/measure_duty_expression_presenter.h"
#include "presenters/measure_legal_act_presenter.h"
#include "presenters/measure_presenter.h"
#include "presenters/measure_type_presenter.h"
#include "presenters/measurement_unit_presenter.h"
#include "presenters/measurement_unit_qualifier_presenter.h"
#include "presenters/quota_closed_and_transferred_event_presenter.h"
#include "presenters/quota_definition_presenter.h"
#include "presenters/quota_order_number_presenter.h"
#include "renderers/additional_code_jsonapi_renderer.h"
#include "renderers/classification_jsonapi_renderer.h"
#include "renderers/duty_expression_jsonapi_renderer.h"
#include "renderers/footnote_jsonapi_renderer.h"
#include "renderers/geographical_area_jsonapi_renderer.h"
#include "renderers/guide_jsonapi_renderer.h"
#include "renderers/import_trade_summary_jsonapi_renderer.h"
#include "renderers/jsonapi_relationship.h"
#include "renderers/measure_component_jsonapi_renderer.h"
#include "renderers/measure_condition_component_jsonapi_renderer.h"
#include "renderers/measure_condition_code_jsonapi_renderer.h"
#include "renderers/measure_condition_jsonapi_renderer.h"
#include "renderers/measure_condition_permutation_jsonapi_renderer.h"
#include "renderers/measure_jsonapi_renderer.h"
#include "renderers/measure_legal_act_jsonapi_renderer.h"
#include "renderers/measure_type_jsonapi_renderer.h"
#include "renderers/measurement_unit_jsonapi_renderer.h"
#include "renderers/measurement_unit_qualifier_jsonapi_renderer.h"
#include "renderers/quota_closed_and_transferred_event_jsonapi_renderer.h"
#include "renderers/quota_definition_jsonapi_renderer.h"
#include "renderers/quota_order_number_jsonapi_renderer.h"

#include <stdlib.h>
#include <string.h>
#include <yyjson.h>

static int add_string_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
}

static int add_int_string_or_null(yyjson_mut_doc *doc,
                                  yyjson_mut_val *obj,
                                  const char *key,
                                  const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }

    char *end = NULL;
    long parsed = strtol(value, &end, 10);
    if (end == value || (end != NULL && *end != '\0')) {
        return 0;
    }

    return yyjson_mut_obj_add_int(doc, obj, key, (int)parsed);
}

static int add_metadata_string_field(yyjson_mut_doc *doc,
                                     yyjson_mut_val *obj,
                                     const char *key,
                                     const DutyCalculatorMetadataField *field)
{
    if (field == NULL || !field->present) {
        return 1;
    }
    if (field->is_null) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, field->value);
}

static yyjson_mut_val *preference_code_resource(yyjson_mut_doc *doc, const char *code)
{
    const char *description = preference_code_presenter_description(code);
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       preference_code_presenter_resource_type(),
                                                       preference_code_presenter_id(code));
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (description == NULL ||
        resource == NULL ||
        attributes == NULL ||
        !yyjson_mut_obj_add_strcpy(doc, attributes, "code", code) ||
        !yyjson_mut_obj_add_strcpy(doc, attributes, "description", description) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        return NULL;
    }
    return resource;
}

static int append_included_resource(JsonApiIncludeIndex *index,
                                    yyjson_mut_val *included,
                                    const char *type,
                                    const char *id,
                                    yyjson_mut_val *resource)
{
    return jsonapi_include_index_append_resource(index, included, type, id, resource);
}

static int include_enabled(const CommodityIncludePlan *plan, CommodityIncludeId include_id)
{
    if (plan == NULL) {
        CommodityIncludePlan default_plan = commodity_include_plan_default();
        return commodity_include_plan_enabled(&default_plan, include_id);
    }
    return commodity_include_plan_enabled(plan, include_id);
}

static int include_for_measure(const CommodityIncludePlan *plan,
                               const MeasureModel *measure,
                               CommodityIncludeId import_include_id,
                               CommodityIncludeId export_include_id)
{
    return (measure_model_import(measure) && include_enabled(plan, import_include_id)) ||
           (measure_model_export(measure) && include_enabled(plan, export_include_id));
}

static const MeasureModel *measure_for_sid(const MeasureModelList *measures, const char *measure_sid)
{
    if (measures == NULL || measure_sid == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < measures->len; i++) {
        if (measures->items[i].measure_sid != NULL &&
            strcmp(measures->items[i].measure_sid, measure_sid) == 0) {
            return &measures->items[i];
        }
    }
    return NULL;
}

static int preference_code_seen(const MeasureModelList *measures,
                                const MeasureConditionModelList *conditions,
                                const PreferenceCodeDeclarable *declarable,
                                const char *code,
                                size_t before_index)
{
    for (size_t i = 0; i < before_index; i++) {
        const char *seen_code = preference_code_presenter_code_for_measure(declarable,
                                                                           &measures->items[i],
                                                                           conditions);
        if (seen_code != NULL && strcmp(seen_code, code) == 0) {
            return 1;
        }
    }
    return 0;
}

static char *presented_description(const GoodsNomenclatureTreeNode *node, const char *service)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(node->description, service);
    return goods_nomenclature_description_presenter_description(&model);
}

static char *presented_description_plain(const GoodsNomenclatureTreeNode *node, const char *service)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(node->description, service);
    return goods_nomenclature_description_presenter_description_plain(&model);
}

static char *presented_formatted_description(const GoodsNomenclatureTreeNode *node, const char *service)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(node->description, service);
    return goods_nomenclature_description_presenter_formatted_description(&model);
}

static yyjson_mut_val *commodity_attributes(yyjson_mut_doc *doc,
                                            const GoodsNomenclatureTreeNode *node,
                                            const char *service,
                                            const CommodityAggregate *aggregate,
                                            int root_resource)
{
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *description = presented_description(node, service);
    char *description_plain = presented_description_plain(node, service);
    char *formatted_description = presented_formatted_description(node, service);

    if (attributes == NULL || description == NULL || description_plain == NULL || formatted_description == NULL) {
        free(description);
        free(description_plain);
        free(formatted_description);
        return NULL;
    }

    int ok =
        yyjson_mut_obj_add_strcpy(doc, attributes, "producline_suffix", node->producline_suffix) &&
        add_string_or_null(doc, attributes, "description", description) &&
        add_int_string_or_null(doc, attributes, "number_indents", node->number_indents) &&
        yyjson_mut_obj_add_strcpy(doc, attributes, "goods_nomenclature_item_id", node->goods_nomenclature_item_id) &&
        add_string_or_null(doc, attributes, "formatted_description", formatted_description) &&
        add_string_or_null(doc, attributes, "description_plain", description_plain) &&
        add_string_or_null(doc, attributes, "validity_start_date", node->validity_start_date) &&
        add_string_or_null(doc, attributes, "validity_end_date", node->validity_end_date);

    if (ok && root_resource) {
        char *basic_duty_rate = commodity_presenter_basic_duty_rate(aggregate);
        GoodsNomenclatureDescriptionModel description_model = goods_nomenclature_description_new(node->description, service);
        char *consigned_from = goods_nomenclature_description_presenter_consigned_from(&description_model);
        ok =
            yyjson_mut_obj_add_strcpy(doc,
                                      attributes,
                                      "bti_url",
                                      "https://www.gov.uk/guidance/check-what-youll-need-to-get-a-legally-binding-decision-on-a-commodity-code") &&
            yyjson_mut_obj_add_bool(doc, attributes, "consigned", consigned_from != NULL) &&
            add_string_or_null(doc, attributes, "consigned_from", consigned_from) &&
            add_string_or_null(doc, attributes, "basic_duty_rate", basic_duty_rate) &&
            yyjson_mut_obj_add_bool(doc, attributes, "meursing_code", commodity_presenter_meursing_code(aggregate)) &&
            yyjson_mut_obj_add_bool(doc,
                                    attributes,
                                    "declarable",
                                    strcmp(node->producline_suffix, "80") == 0 && node->leaf) &&
            yyjson_mut_obj_add_bool(doc, attributes, "has_chemicals", node->has_chemicals != 0);
        free(consigned_from);
        free(basic_duty_rate);
    }

    free(description);
    free(description_plain);
    free(formatted_description);
    return ok ? attributes : NULL;
}

static yyjson_mut_val *commodity_meta(yyjson_mut_doc *doc,
                                      const CommodityAggregate *aggregate,
                                      const char *service)
{
    yyjson_mut_val *meta = yyjson_mut_obj(doc);
    yyjson_mut_val *duty_calculator = yyjson_mut_obj(doc);
    yyjson_mut_val *applicable_additional_codes = yyjson_mut_obj(doc);
    yyjson_mut_val *applicable_measure_units = yyjson_mut_obj(doc);
    yyjson_mut_val *applicable_vat_options = yyjson_mut_obj(doc);
    if (meta == NULL || duty_calculator == NULL || applicable_additional_codes == NULL || applicable_measure_units == NULL || applicable_vat_options == NULL) {
        return NULL;
    }

    DutyCalculatorAdditionalCodeGroupModelList additional_code_groups =
        commodity_presenter_applicable_additional_codes(aggregate);
    for (size_t i = 0; i < additional_code_groups.len; i++) {
        const DutyCalculatorAdditionalCodeGroupModel *group = &additional_code_groups.items[i];
        yyjson_mut_val *group_key = yyjson_mut_strcpy(doc, group->measure_type_id);
        yyjson_mut_val *group_obj = yyjson_mut_obj(doc);
        yyjson_mut_val *annotations = yyjson_mut_arr(doc);
        if (group_key == NULL ||
            group_obj == NULL ||
            annotations == NULL ||
            !yyjson_mut_obj_add_strcpy(doc, group_obj, "measure_type_description", group->measure_type_description)) {
            duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
            return NULL;
        }
        if (group->heading_overlay == NULL && group->heading_hint == NULL) {
            if (!yyjson_mut_obj_add_null(doc, group_obj, "heading")) {
                duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
                return NULL;
            }
        } else {
            yyjson_mut_val *heading = yyjson_mut_obj(doc);
            if (heading == NULL ||
                !yyjson_mut_obj_add_strcpy(doc, heading, "overlay", group->heading_overlay) ||
                !yyjson_mut_obj_add_strcpy(doc, heading, "hint", group->heading_hint) ||
                !yyjson_mut_obj_add_val(doc, group_obj, "heading", heading)) {
                duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
                return NULL;
            }
        }

        for (size_t j = 0; j < group->additional_codes.len; j++) {
            const DutyCalculatorAdditionalCodeAnnotationModel *annotation = &group->additional_codes.items[j];
            yyjson_mut_val *annotation_obj = yyjson_mut_obj(doc);
            if (annotation_obj == NULL ||
                !yyjson_mut_obj_add_strcpy(doc, annotation_obj, "code", annotation->code) ||
                !yyjson_mut_obj_add_strcpy(doc, annotation_obj, "overlay", annotation->overlay) ||
                !yyjson_mut_obj_add_strcpy(doc, annotation_obj, "hint", annotation->hint) ||
                !yyjson_mut_obj_add_strcpy(doc, annotation_obj, "geographical_area_id", annotation->geographical_area_id) ||
                !yyjson_mut_obj_add_strcpy(doc, annotation_obj, "measure_sid", annotation->measure_sid) ||
                !yyjson_mut_arr_append(annotations, annotation_obj)) {
                duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
                return NULL;
            }
        }

        if (!yyjson_mut_obj_add_val(doc, group_obj, "additional_codes", annotations) ||
            !yyjson_mut_obj_add(applicable_additional_codes, group_key, group_obj)) {
            duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
            return NULL;
        }
    }

    DutyCalculatorMeasureUnitModelList measure_units = commodity_presenter_applicable_measure_units(aggregate);
    for (size_t i = 0; i < measure_units.len; i++) {
        const DutyCalculatorMeasureUnitModel *measure_unit = &measure_units.items[i];
        yyjson_mut_val *key = yyjson_mut_strcpy(doc, measure_unit->key);
        yyjson_mut_val *value = yyjson_mut_obj(doc);
        if (key == NULL ||
            value == NULL ||
            !add_metadata_string_field(doc, value, "measurement_unit_code", &measure_unit->measurement_unit_code) ||
            !add_metadata_string_field(doc, value, "measurement_unit_qualifier_code", &measure_unit->measurement_unit_qualifier_code) ||
            !add_metadata_string_field(doc, value, "abbreviation", &measure_unit->abbreviation) ||
            !add_metadata_string_field(doc, value, "expansion", &measure_unit->expansion) ||
            !add_metadata_string_field(doc, value, "unit_question", &measure_unit->unit_question) ||
            !add_metadata_string_field(doc, value, "unit_hint", &measure_unit->unit_hint) ||
            !add_metadata_string_field(doc, value, "unit", &measure_unit->unit) ||
            !add_metadata_string_field(doc, value, "multiplier", &measure_unit->multiplier) ||
            !add_metadata_string_field(doc, value, "coerced_measurement_unit_code", &measure_unit->coerced_measurement_unit_code) ||
            !add_metadata_string_field(doc, value, "original_unit", &measure_unit->original_unit) ||
            !add_metadata_string_field(doc, value, "measurement_unit_type", &measure_unit->measurement_unit_type)) {
            duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
            duty_calculator_measure_unit_model_list_free(&measure_units);
            return NULL;
        }
        if (!yyjson_mut_obj_add(applicable_measure_units, key, value)) {
            duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
            duty_calculator_measure_unit_model_list_free(&measure_units);
            return NULL;
        }
    }

    DutyCalculatorVatOptionModelList vat_options = commodity_presenter_applicable_vat_options(aggregate);
    for (size_t i = 0; i < vat_options.len; i++) {
        yyjson_mut_val *key = yyjson_mut_strcpy(doc, vat_options.items[i].key);
        yyjson_mut_val *value = yyjson_mut_strcpy(doc, vat_options.items[i].description);
        if (key == NULL ||
            value == NULL ||
            !yyjson_mut_obj_add(applicable_vat_options, key, value)) {
            duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
            duty_calculator_measure_unit_model_list_free(&measure_units);
            duty_calculator_vat_option_model_list_free(&vat_options);
            return NULL;
        }
    }

    if (!yyjson_mut_obj_add_bool(doc,
                                 duty_calculator,
                                 "entry_price_system",
                                 commodity_presenter_entry_price_system(aggregate, service)) ||
        !yyjson_mut_obj_add_val(doc,
                                duty_calculator,
                                "applicable_additional_codes",
                                applicable_additional_codes) ||
        !yyjson_mut_obj_add_val(doc,
                                duty_calculator,
                                "applicable_measure_units",
                                applicable_measure_units) ||
        !yyjson_mut_obj_add_val(doc,
                                duty_calculator,
                                "applicable_vat_options",
                                applicable_vat_options) ||
        !yyjson_mut_obj_add_bool(doc,
                                 duty_calculator,
                                 "meursing_code",
                                 commodity_presenter_meursing_code(aggregate)) ||
        !yyjson_mut_obj_add_strcpy(doc, duty_calculator, "source", service) ||
        !yyjson_mut_obj_add_bool(doc,
                                 duty_calculator,
                                 "trade_defence",
                                 commodity_presenter_trade_defence(aggregate)) ||
        !yyjson_mut_obj_add_bool(doc,
                                 duty_calculator,
                                 "zero_mfn_duty",
                                 commodity_presenter_zero_mfn_duty(aggregate)) ||
        !yyjson_mut_obj_add_val(doc, meta, "duty_calculator", duty_calculator)) {
        duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
        duty_calculator_measure_unit_model_list_free(&measure_units);
        duty_calculator_vat_option_model_list_free(&vat_options);
        return NULL;
    }

    duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
    duty_calculator_measure_unit_model_list_free(&measure_units);
    duty_calculator_vat_option_model_list_free(&vat_options);
    return meta;
}

static yyjson_mut_val *ancestor_relationship(yyjson_mut_doc *doc,
                                             const GoodsNomenclatureTreeNodeList *ancestors)
{
    yyjson_mut_val *relationship = yyjson_mut_obj(doc);
    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (relationship == NULL || data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < ancestors->len; i++) {
        const GoodsNomenclatureTreeNode *ancestor = &ancestors->items[i];

        if (!goods_nomenclature_tree_node_is_ten_digit_goods_nomenclature(ancestor)) {
            continue;
        }

        yyjson_mut_val *ref = jsonapi_resource_identifier(
            doc,
            commodity_presenter_resource_type(),
            commodity_presenter_tree_node_id(ancestor));
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return yyjson_mut_obj_add_val(doc, relationship, "data", data) ? relationship : NULL;
}

static yyjson_mut_val *footnote_relationship_data(yyjson_mut_doc *doc,
                                                  const FootnoteList *footnotes)
{
    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL || footnotes == NULL) {
        return data;
    }

    for (size_t i = 0; i < footnotes->len; i++) {
        yyjson_mut_val *ref = footnote_jsonapi_resource_identifier(doc, &footnotes->items[i]);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

static yyjson_mut_val *commodity_resource(yyjson_mut_doc *doc,
                                          const GoodsNomenclatureTreeNode *node,
                                          const char *service,
                                          const CommodityAggregate *aggregate,
                                          int root_resource)
{
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       commodity_presenter_resource_type(),
                                                       commodity_presenter_tree_node_id(node));
    yyjson_mut_val *attributes = commodity_attributes(doc, node, service, aggregate, root_resource);
    if (resource == NULL || attributes == NULL ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        return NULL;
    }

    if (root_resource) {
        yyjson_mut_val *meta = commodity_meta(doc, aggregate, service);
        yyjson_mut_val *relationships = yyjson_mut_obj(doc);
        yyjson_mut_val *section_rel = jsonapi_nullable_relationship(
            doc,
            classification_section_presenter_resource_type(),
            aggregate->classification.has_section
                ? classification_section_presenter_id(&aggregate->classification.section)
                : NULL);
        yyjson_mut_val *chapter_rel = jsonapi_nullable_relationship(
            doc,
            classification_chapter_presenter_resource_type(),
            aggregate->classification.has_chapter
                ? classification_chapter_presenter_id(&aggregate->classification.chapter)
                : NULL);
        yyjson_mut_val *heading_rel = jsonapi_nullable_relationship(
            doc,
            classification_heading_presenter_resource_type(),
            aggregate->classification.has_heading
                ? classification_heading_presenter_id(&aggregate->classification.heading)
                : NULL);
        yyjson_mut_val *ancestor_rel = ancestor_relationship(doc, &aggregate->ancestors);
        yyjson_mut_val *footnote_data = footnote_relationship_data(doc, &aggregate->commodity_footnotes);
        yyjson_mut_val *import_trade_summary_data = import_trade_summary_jsonapi_resource_identifier(doc,
                                                                                                     &aggregate->import_trade_summary);
        yyjson_mut_val *import_measure_data = measure_jsonapi_relationship_data(doc, &aggregate->applicable_measures, 1);
        yyjson_mut_val *export_measure_data = measure_jsonapi_relationship_data(doc, &aggregate->applicable_measures, 0);
        if (meta == NULL ||
            relationships == NULL ||
            section_rel == NULL ||
            chapter_rel == NULL ||
            heading_rel == NULL ||
            ancestor_rel == NULL ||
            footnote_data == NULL ||
            import_trade_summary_data == NULL ||
            import_measure_data == NULL ||
            export_measure_data == NULL ||
            !yyjson_mut_obj_add_val(doc, relationships, "section", section_rel) ||
            !yyjson_mut_obj_add_val(doc, relationships, "chapter", chapter_rel) ||
            !yyjson_mut_obj_add_val(doc, relationships, "heading", heading_rel) ||
            !yyjson_mut_obj_add_val(doc, relationships, "ancestors", ancestor_rel) ||
            !jsonapi_add_data_relationship(doc, relationships, "footnotes", footnote_data) ||
            !jsonapi_add_data_relationship(doc, relationships, "import_trade_summary", import_trade_summary_data) ||
            !jsonapi_add_data_relationship(doc, relationships, "import_measures", import_measure_data) ||
            !jsonapi_add_data_relationship(doc, relationships, "export_measures", export_measure_data) ||
            !yyjson_mut_obj_add_val(doc, resource, "meta", meta) ||
            !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
            return NULL;
        }
    }

    return resource;
}

char *commodity_tree_slice_render_jsonapi_with_plan(const CommodityAggregate *aggregate,
                                                    const char *service,
                                                    const CommodityIncludePlan *plan)
{
    if (aggregate == NULL || aggregate->commodity.goods_nomenclature_sid == NULL) {
        return NULL;
    }

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    if (doc == NULL) {
        return NULL;
    }

    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_val *data = commodity_resource(doc,
                                              &aggregate->commodity,
                                              service,
                                              aggregate,
                                              1);
    yyjson_mut_val *included = yyjson_mut_arr(doc);
    JsonApiIncludeIndex included_index = { 0 };
    if (root == NULL || data == NULL || included == NULL) {
        jsonapi_include_index_free(&included_index);
        yyjson_mut_doc_free(doc);
        return NULL;
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_SECTION) && aggregate->classification.has_section) {
        yyjson_mut_val *resource = classification_section_jsonapi_resource(doc,
                                                                           &aggregate->classification.section);
        if (resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      classification_section_presenter_resource_type(),
                                      classification_section_presenter_id(&aggregate->classification.section),
                                      resource)) {
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_CHAPTER) && aggregate->classification.has_chapter) {
        yyjson_mut_val *resource = classification_chapter_jsonapi_resource(doc,
                                                                           &aggregate->classification.chapter,
                                                                           &aggregate->chapter_guides,
                                                                           service);
        if (resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      classification_chapter_presenter_resource_type(),
                                      classification_chapter_presenter_id(&aggregate->classification.chapter),
                                      resource)) {
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_CHAPTER_GUIDES)) {
        for (size_t i = 0; i < aggregate->chapter_guides.len; i++) {
            yyjson_mut_val *resource = guide_jsonapi_resource(doc, &aggregate->chapter_guides.items[i]);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          guide_presenter_resource_type(),
                                          guide_presenter_id(&aggregate->chapter_guides.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_HEADING) && aggregate->classification.has_heading) {
        yyjson_mut_val *resource = classification_heading_jsonapi_resource(doc,
                                                                           &aggregate->classification.heading,
                                                                           service);
        if (resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      classification_heading_presenter_resource_type(),
                                      classification_heading_presenter_id(&aggregate->classification.heading),
                                      resource)) {
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_ANCESTORS)) {
        for (size_t i = 0; i < aggregate->ancestors.len; i++) {
            const GoodsNomenclatureTreeNode *ancestor = &aggregate->ancestors.items[i];

            if (!goods_nomenclature_tree_node_is_ten_digit_goods_nomenclature(ancestor)) {
                continue;
            }

            yyjson_mut_val *resource = commodity_resource(doc,
                                                          ancestor,
                                                          service,
                                                          aggregate,
                                                          0);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          commodity_presenter_resource_type(),
                                          commodity_presenter_tree_node_id(ancestor),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    PreferenceCodeDeclarable preference_declarable =
        preference_code_presenter_declarable_from_measures(&aggregate->applicable_measures,
                                                           &aggregate->measure_conditions);
    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        if (!include_for_measure(plan,
                                 &aggregate->applicable_measures.items[i],
                                 COMMODITY_INCLUDE_IMPORT_MEASURES,
                                 COMMODITY_INCLUDE_EXPORT_MEASURES)) {
            continue;
        }
        yyjson_mut_val *resource = measure_jsonapi_resource(doc,
                                                            &aggregate->applicable_measures.items[i],
                                                            &preference_declarable,
                                                            &aggregate->measure_components,
                                                            &aggregate->resolved_measure_components,
                                                            &aggregate->measure_conditions,
                                                            &aggregate->measure_condition_permutation_groups,
                                                            &aggregate->measure_condition_components,
                                                            &aggregate->appendix_5as,
                                                            &aggregate->geographical_areas,
                                                            &aggregate->excluded_geographical_areas,
                                                            &aggregate->measure_footnotes,
                                                            &aggregate->measure_legal_acts);
        if (resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      measure_presenter_resource_type(),
                                      measure_presenter_id(&aggregate->applicable_measures.items[i]),
                                      resource)) {
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_PREFERENCE_CODE)) {
        for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
            const char *code = preference_code_presenter_code_for_measure(&preference_declarable,
                                                                          &aggregate->applicable_measures.items[i],
                                                                          &aggregate->measure_conditions);
            if (code == NULL ||
                preference_code_seen(&aggregate->applicable_measures,
                                     &aggregate->measure_conditions,
                                     &preference_declarable,
                                     code,
                                     i)) {
                continue;
            }
            yyjson_mut_val *resource = preference_code_resource(doc, code);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          preference_code_presenter_resource_type(),
                                          preference_code_presenter_id(code),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        if (!include_for_measure(plan,
                                 &aggregate->applicable_measures.items[i],
                                 COMMODITY_INCLUDE_IMPORT_MEASURES_DUTY_EXPRESSION,
                                 COMMODITY_INCLUDE_EXPORT_MEASURES_DUTY_EXPRESSION)) {
            continue;
        }
        char *id = measure_duty_expression_presenter_id(&aggregate->applicable_measures.items[i]);
        yyjson_mut_val *resource = duty_expression_jsonapi_resource(doc,
                                                                    &aggregate->applicable_measures.items[i],
                                                                    &aggregate->measure_components);
        if (id == NULL ||
            resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      duty_expression_presenter_resource_type(),
                                      id,
                                      resource)) {
            free(id);
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
        free(id);
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_TYPE) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_TYPE)) {
        for (size_t i = 0; i < aggregate->measure_types.len; i++) {
            yyjson_mut_val *resource = measure_type_jsonapi_resource(doc, &aggregate->measure_types.items[i]);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measure_type_presenter_resource_type(),
                                          measure_type_presenter_id(&aggregate->measure_types.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_TRADE_SUMMARY)) {
        yyjson_mut_val *import_trade_summary = import_trade_summary_jsonapi_resource(doc, &aggregate->import_trade_summary);
        if (import_trade_summary == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      import_trade_summary_presenter_resource_type(),
                                      import_trade_summary_presenter_id(&aggregate->import_trade_summary),
                                      import_trade_summary)) {
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
    }

    for (size_t i = 0; i < aggregate->measure_components.len; i++) {
        const MeasureModel *measure = measure_for_sid(&aggregate->applicable_measures,
                                                      aggregate->measure_components.items[i].measure_sid);
        if (!include_for_measure(plan,
                                 measure,
                                 COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS,
                                 COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS)) {
            continue;
        }
        char *id = measure_component_presenter_id(&aggregate->measure_components.items[i]);
        yyjson_mut_val *resource = measure_component_jsonapi_resource(doc, &aggregate->measure_components.items[i]);
        if (id == NULL ||
            resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      measure_component_presenter_resource_type(),
                                      id,
                                      resource)) {
            free(id);
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
        free(id);
    }

    for (size_t i = 0; i < aggregate->resolved_measure_components.len; i++) {
        const MeasureModel *measure = measure_for_sid(&aggregate->applicable_measures,
                                                      aggregate->resolved_measure_components.items[i].measure_sid);
        if (!include_for_measure(plan,
                                 measure,
                                 COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS,
                                 COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS)) {
            continue;
        }
        char *id = measure_component_presenter_id(&aggregate->resolved_measure_components.items[i]);
        yyjson_mut_val *resource = measure_component_jsonapi_resource(doc, &aggregate->resolved_measure_components.items[i]);
        if (id == NULL ||
            resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      measure_component_presenter_resource_type(),
                                      id,
                                      resource)) {
            free(id);
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
        free(id);
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT) ||
        include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_RESOLVED_MEASURE_COMPONENTS_MEASUREMENT_UNIT)) {
        for (size_t i = 0; i < aggregate->measurement_units.len; i++) {
            yyjson_mut_val *resource = measurement_unit_jsonapi_resource(doc, &aggregate->measurement_units.items[i]);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measurement_unit_presenter_resource_type(),
                                          measurement_unit_presenter_id(&aggregate->measurement_units.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT_QUALIFIER) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_COMPONENTS_MEASUREMENT_UNIT_QUALIFIER)) {
        for (size_t i = 0; i < aggregate->measurement_unit_qualifiers.len; i++) {
            yyjson_mut_val *resource = measurement_unit_qualifier_jsonapi_resource(doc,
                                                                                   &aggregate->measurement_unit_qualifiers.items[i],
                                                                                   service);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measurement_unit_qualifier_presenter_resource_type(),
                                          measurement_unit_qualifier_presenter_id(&aggregate->measurement_unit_qualifiers.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    for (size_t i = 0; i < aggregate->measure_conditions.len; i++) {
        const MeasureModel *measure = measure_for_sid(&aggregate->applicable_measures,
                                                      aggregate->measure_conditions.items[i].measure_sid);
        if (!include_for_measure(plan,
                                 measure,
                                 COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS,
                                 COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS)) {
            continue;
        }
        yyjson_mut_val *resource = measure_condition_jsonapi_resource(doc,
                                                                      &aggregate->measure_conditions.items[i],
                                                                      &aggregate->measure_condition_components,
                                                                      &aggregate->appendix_5as);
        if (resource == NULL ||
            !append_included_resource(&included_index,
                                      included,
                                      measure_condition_presenter_resource_type(),
                                      measure_condition_presenter_id(&aggregate->measure_conditions.items[i]),
                                      resource)) {
            jsonapi_include_index_free(&included_index);
            yyjson_mut_doc_free(doc);
            return NULL;
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_CODE) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_CODE)) {
        for (size_t i = 0; i < aggregate->measure_condition_codes.len; i++) {
            yyjson_mut_val *resource = measure_condition_code_jsonapi_resource(
                doc,
                &aggregate->measure_condition_codes.items[i]);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measure_condition_code_presenter_resource_type(),
                                          measure_condition_code_presenter_id(&aggregate->measure_condition_codes.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_COMPONENTS) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITIONS_MEASURE_CONDITION_COMPONENTS)) {
        for (size_t i = 0; i < aggregate->measure_condition_components.len; i++) {
            char *id = measure_condition_component_presenter_id(&aggregate->measure_condition_components.items[i]);
            yyjson_mut_val *resource = measure_condition_component_jsonapi_resource(
                doc,
                &aggregate->measure_condition_components.items[i]);
            if (id == NULL ||
                resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measure_condition_component_presenter_resource_type(),
                                          id,
                                          resource)) {
                free(id);
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
            free(id);
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS)) {
        for (size_t i = 0; i < aggregate->measure_condition_permutation_groups.len; i++) {
            yyjson_mut_val *resource = measure_condition_permutation_group_jsonapi_resource(
                doc,
                &aggregate->measure_condition_permutation_groups.items[i]);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measure_condition_permutation_group_presenter_resource_type(),
                                          measure_condition_permutation_group_presenter_id(&aggregate->measure_condition_permutation_groups.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS_PERMUTATIONS) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_MEASURE_CONDITION_PERMUTATION_GROUPS_PERMUTATIONS)) {
        for (size_t i = 0; i < aggregate->measure_condition_permutation_groups.len; i++) {
            const MeasureConditionPermutationGroupModel *group = &aggregate->measure_condition_permutation_groups.items[i];
            for (size_t j = 0; j < group->permutations.len; j++) {
                yyjson_mut_val *resource = measure_condition_permutation_jsonapi_resource(
                    doc,
                    &group->permutations.items[j]);
                if (resource == NULL ||
                    !append_included_resource(&included_index,
                                              included,
                                              measure_condition_permutation_presenter_resource_type(),
                                              measure_condition_permutation_presenter_id(&group->permutations.items[j]),
                                              resource)) {
                    jsonapi_include_index_free(&included_index);
                    yyjson_mut_doc_free(doc);
                    return NULL;
                }
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_GEOGRAPHICAL_AREA) ||
        include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_GEOGRAPHICAL_AREA_CONTAINED_GEOGRAPHICAL_AREAS) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_GEOGRAPHICAL_AREA) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_GEOGRAPHICAL_AREA_CONTAINED_GEOGRAPHICAL_AREAS)) {
        for (size_t i = 0; i < aggregate->geographical_areas.len; i++) {
            yyjson_mut_val *resource = geographical_area_jsonapi_resource(doc,
                                                                          &aggregate->geographical_areas.items[i],
                                                                          &aggregate->geographical_areas);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          geographical_area_presenter_resource_type(),
                                          geographical_area_presenter_id(&aggregate->geographical_areas.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_ADDITIONAL_CODE) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_ADDITIONAL_CODE)) {
        for (size_t i = 0; i < aggregate->additional_codes.len; i++) {
            yyjson_mut_val *resource = additional_code_jsonapi_resource(doc, &aggregate->additional_codes.items[i]);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          additional_code_presenter_resource_type(),
                                          additional_code_presenter_id(&aggregate->additional_codes.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_FOOTNOTES)) {
        for (size_t i = 0; i < aggregate->commodity_footnotes.len; i++) {
            yyjson_mut_val *resource = footnote_jsonapi_resource(doc, &aggregate->commodity_footnotes.items[i], service);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          footnote_presenter_resource_type(),
                                          footnote_presenter_id(&aggregate->commodity_footnotes.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_FOOTNOTES) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_FOOTNOTES)) {
        for (size_t i = 0; i < aggregate->measure_footnotes.len; i++) {
            yyjson_mut_val *resource = footnote_jsonapi_resource(doc, &aggregate->measure_footnotes.items[i], service);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          footnote_presenter_resource_type(),
                                          footnote_presenter_id(&aggregate->measure_footnotes.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_LEGAL_ACTS) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_LEGAL_ACTS)) {
        for (size_t i = 0; i < aggregate->measure_legal_acts.len; i++) {
            yyjson_mut_val *resource = measure_legal_act_jsonapi_resource(doc,
                                                                          &aggregate->measure_legal_acts.items[i],
                                                                          service);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measure_legal_act_presenter_resource_type(),
                                          measure_legal_act_presenter_id(&aggregate->measure_legal_acts.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_SUSPENDING_REGULATION) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_SUSPENDING_REGULATION)) {
        for (size_t i = 0; i < aggregate->measure_legal_acts.len; i++) {
            const MeasureLegalActModel *legal_act = &aggregate->measure_legal_acts.items[i];
            const MeasureModel *measure = measure_for_sid(&aggregate->applicable_measures,
                                                          legal_act->measure_sid);
            if (legal_act->source == NULL ||
                strcmp(legal_act->source, "suspension") != 0 ||
                !include_for_measure(plan,
                                     measure,
                                     COMMODITY_INCLUDE_IMPORT_MEASURES_SUSPENDING_REGULATION,
                                     COMMODITY_INCLUDE_EXPORT_MEASURES_SUSPENDING_REGULATION)) {
                continue;
            }

            yyjson_mut_val *resource = measure_legal_act_jsonapi_suspension_resource(doc, legal_act);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          measure_legal_act_presenter_suspension_resource_type(),
                                          measure_legal_act_presenter_id(legal_act),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER)) {
        for (size_t i = 0; i < aggregate->quota_order_numbers.len; i++) {
            yyjson_mut_val *resource = quota_order_number_jsonapi_resource(doc,
                                                                           &aggregate->quota_order_numbers.items[i],
                                                                           &aggregate->quota_definitions);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          quota_order_number_presenter_resource_type(),
                                          quota_order_number_presenter_id(&aggregate->quota_order_numbers.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER_DEFINITION) ||
        include_enabled(plan, COMMODITY_INCLUDE_EXPORT_MEASURES_ORDER_NUMBER_DEFINITION)) {
        for (size_t i = 0; i < aggregate->quota_definitions.len; i++) {
            yyjson_mut_val *resource = quota_definition_jsonapi_resource_with_events(
                doc,
                &aggregate->quota_definitions.items[i],
                &aggregate->quota_events,
                &aggregate->quota_closed_and_transferred_events);
            if (resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          quota_definition_presenter_jsonapi_resource_type(),
                                          quota_definition_presenter_id(&aggregate->quota_definitions.items[i]),
                                          resource)) {
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
        }
    }

    if (include_enabled(plan, COMMODITY_INCLUDE_IMPORT_MEASURES_ORDER_NUMBER_DEFINITION_INCOMING_QUOTA_CLOSED_AND_TRANSFERRED_EVENT)) {
        for (size_t i = 0; i < aggregate->quota_closed_and_transferred_events.len; i++) {
            char *id = quota_closed_and_transferred_event_presenter_id(&aggregate->quota_closed_and_transferred_events.items[i]);
            yyjson_mut_val *resource = quota_closed_and_transferred_event_jsonapi_resource(
                doc,
                &aggregate->quota_closed_and_transferred_events.items[i]);
            if (id == NULL ||
                resource == NULL ||
                !append_included_resource(&included_index,
                                          included,
                                          quota_closed_and_transferred_event_presenter_resource_type(),
                                          id,
                                          resource)) {
                free(id);
                jsonapi_include_index_free(&included_index);
                yyjson_mut_doc_free(doc);
                return NULL;
            }
            free(id);
        }
    }

    if (!yyjson_mut_obj_add_val(doc, root, "data", data) ||
        !yyjson_mut_obj_add_val(doc, root, "included", included)) {
        jsonapi_include_index_free(&included_index);
        yyjson_mut_doc_free(doc);
        return NULL;
    }

    yyjson_mut_doc_set_root(doc, root);
    size_t len = 0;
    char *json = yyjson_mut_write(doc, YYJSON_WRITE_NOFLAG, &len);
    jsonapi_include_index_free(&included_index);
    yyjson_mut_doc_free(doc);
    return json;
}

char *commodity_tree_slice_render_jsonapi(const CommodityAggregate *aggregate,
                                          const char *service)
{
    CommodityIncludePlan plan = commodity_include_plan_default();
    return commodity_tree_slice_render_jsonapi_with_plan(aggregate, service, &plan);
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

int commodity_tree_slice_renderer_self_test(void)
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
            .description = copy_literal("Electronic cigarettes"),
            .leaf = 1,
        },
        .import_trade_summary = {
            .id = copy_literal("summary-id"),
        },
    };

    char *json = commodity_tree_slice_render_jsonapi(&aggregate, "uk");
    int ok = json != NULL &&
             strstr(json, "\"id\":\"123456\"") != NULL &&
             strstr(json, "\"declarable\":true") != NULL &&
             strstr(json, "\"import_trade_summary\"") != NULL &&
             strstr(json, "\"ancestors\":{\"data\":[]}") != NULL;

    free(json);
    commodity_aggregate_free(&aggregate);
    return ok;
}
