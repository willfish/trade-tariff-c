#include "presenters/commodity_presenter.h"

#include "presenters/measure_duty_expression_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yyjson.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static char *read_file(const char *path, size_t *size_out);

static char *copy_string(const char *value)
{
    if (value == NULL) {
        return NULL;
    }

    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

static char *join_strings(const char *left, const char *right)
{
    const char *safe_left = left == NULL ? "" : left;
    const char *safe_right = right == NULL ? "" : right;
    size_t len = strlen(safe_left) + strlen(safe_right) + 1;
    char *joined = malloc(len);
    if (joined == NULL) {
        return NULL;
    }
    snprintf(joined, len, "%s%s", safe_left, safe_right);
    return joined;
}

const char *commodity_presenter_resource_type(void)
{
    return "commodity";
}

const char *commodity_presenter_id(const CommodityModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

const char *commodity_presenter_tree_node_id(const GoodsNomenclatureTreeNode *node)
{
    return node == NULL ? NULL : node->goods_nomenclature_sid;
}

static int measure_third_country(const MeasureModel *measure)
{
    return measure_model_import(measure) &&
           (streq(measure->measure_type_id, "103") || streq(measure->measure_type_id, "105"));
}

static int measure_trade_defence(const MeasureModel *measure)
{
    static const char *ids[] = { "551", "552", "553", "554", "555", "695", "696" };
    if (!measure_model_import(measure)) {
        return 0;
    }

    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
        if (streq(measure->measure_type_id, ids[i])) {
            return 1;
        }
    }
    return 0;
}

static const MeasureModel *single_third_country_measure(const CommodityAggregate *aggregate)
{
    const MeasureModel *match = NULL;
    size_t count = 0;
    if (aggregate == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (measure_third_country(measure)) {
            match = measure;
            count++;
        }
    }

    return count == 1 ? match : NULL;
}

char *commodity_presenter_basic_duty_rate(const CommodityAggregate *aggregate)
{
    const MeasureModel *measure = single_third_country_measure(aggregate);
    return measure_duty_expression_presenter_formatted(measure,
                                                       aggregate == NULL ? NULL : &aggregate->measure_components);
}

static int duty_expression_meursing(const char *duty_expression_id)
{
    static const char *ids[] = { "12", "14", "21", "25", "27", "29" };
    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
        if (streq(duty_expression_id, ids[i])) {
            return 1;
        }
    }
    return 0;
}

static int measure_has_meursing_component(const CommodityAggregate *aggregate,
                                          const MeasureModel *measure)
{
    if (aggregate == NULL || measure == NULL || measure->measure_sid == NULL) {
        return 0;
    }

    for (size_t i = 0; i < aggregate->measure_components.len; i++) {
        const MeasureComponentModel *component = &aggregate->measure_components.items[i];
        if (streq(component->measure_sid, measure->measure_sid) &&
            duty_expression_meursing(component->duty_expression_id)) {
            return 1;
        }
    }
    return 0;
}

int commodity_presenter_meursing_code(const CommodityAggregate *aggregate)
{
    if (aggregate == NULL) {
        return 0;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (measure_model_import(measure) && measure_has_meursing_component(aggregate, measure)) {
            return 1;
        }
    }
    return 0;
}

static size_t component_count_for_measure(const CommodityAggregate *aggregate,
                                          const MeasureModel *measure)
{
    size_t count = 0;
    if (aggregate == NULL || measure == NULL || measure->measure_sid == NULL) {
        return 0;
    }

    for (size_t i = 0; i < aggregate->measure_components.len; i++) {
        if (streq(aggregate->measure_components.items[i].measure_sid, measure->measure_sid)) {
            count++;
        }
    }
    return count;
}

static const MeasureComponentModel *first_component_for_measure(const CommodityAggregate *aggregate,
                                                                const MeasureModel *measure)
{
    if (aggregate == NULL || measure == NULL || measure->measure_sid == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->measure_components.len; i++) {
        const MeasureComponentModel *component = &aggregate->measure_components.items[i];
        if (streq(component->measure_sid, measure->measure_sid)) {
            return component;
        }
    }
    return NULL;
}

static int component_zero_duty(const MeasureComponentModel *component)
{
    if (component == NULL || component->duty_amount == NULL) {
        return 0;
    }

    char *end = NULL;
    double value = strtod(component->duty_amount, &end);
    return end != component->duty_amount && value == 0.0;
}

static int measure_zero_mfn(const CommodityAggregate *aggregate,
                            const MeasureModel *measure)
{
    return measure_third_country(measure) &&
           component_count_for_measure(aggregate, measure) == 1 &&
           component_zero_duty(first_component_for_measure(aggregate, measure));
}

int commodity_presenter_zero_mfn_duty(const CommodityAggregate *aggregate)
{
    int seen_third_country = 0;
    if (aggregate == NULL) {
        return 0;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (!measure_third_country(measure)) {
            continue;
        }
        seen_third_country = 1;
        if (!measure_zero_mfn(aggregate, measure)) {
            return 0;
        }
    }

    return seen_third_country;
}

int commodity_presenter_trade_defence(const CommodityAggregate *aggregate)
{
    if (aggregate == NULL) {
        return 0;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        if (measure_trade_defence(&aggregate->applicable_measures.items[i])) {
            return 1;
        }
    }
    return 0;
}

int commodity_presenter_entry_price_system(const CommodityAggregate *aggregate,
                                           const char *service)
{
    if (aggregate == NULL || streq(service, "uk")) {
        return 0;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (!measure_model_import(measure) || measure->measure_sid == NULL) {
            continue;
        }

        for (size_t j = 0; j < aggregate->measure_conditions.len; j++) {
            const MeasureConditionModel *condition = &aggregate->measure_conditions.items[j];
            if (streq(condition->measure_sid, measure->measure_sid) &&
                streq(condition->condition_code, "V")) {
                return 1;
            }
        }
    }
    return 0;
}

static int measure_vat(const MeasureModel *measure)
{
    return measure_model_import(measure) &&
           (streq(measure->measure_type_id, "305") || streq(measure->measure_type_id, "VTS"));
}

static const AdditionalCodeModel *additional_code_for_measure(const CommodityAggregate *aggregate,
                                                              const MeasureModel *measure)
{
    if (aggregate == NULL || measure == NULL || measure->additional_code_sid == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->additional_codes.len; i++) {
        const AdditionalCodeModel *code = &aggregate->additional_codes.items[i];
        if (streq(code->additional_code_sid, measure->additional_code_sid)) {
            return code;
        }
    }
    return NULL;
}

static int additional_code_applicable_type(const char *type_id)
{
    return streq(type_id, "2") ||
           streq(type_id, "8") ||
           streq(type_id, "A") ||
           streq(type_id, "B") ||
           streq(type_id, "C") ||
           streq(type_id, "X");
}

static const char *additional_code_type_name(const char *type_id)
{
    if (streq(type_id, "X")) {
        return "excise";
    }
    if (streq(type_id, "2")) {
        return "preference";
    }
    if (streq(type_id, "8") ||
        streq(type_id, "A") ||
        streq(type_id, "B") ||
        streq(type_id, "C")) {
        return "remedy";
    }
    return "unknown";
}

static int append_percent_description(DutyCalculatorVatOptionModel *option,
                                      const char *description,
                                      const char *duty_amount)
{
    if (description == NULL || duty_amount == NULL) {
        return 0;
    }

    size_t len = strlen(description) + strlen(duty_amount) + strlen(" (%)") + 1;
    option->description = malloc(len);
    if (option->description == NULL) {
        return 0;
    }
    snprintf(option->description, len, "%s (%s%%)", description, duty_amount);
    return 1;
}

static void replace_vat_option(DutyCalculatorVatOptionModelList *options,
                               DutyCalculatorVatOptionModel model)
{
    for (size_t i = 0; i < options->len; i++) {
        if (streq(options->items[i].key, model.key)) {
            duty_calculator_vat_option_model_free(&options->items[i]);
            options->items[i] = model;
            return;
        }
    }

    if (!duty_calculator_vat_option_model_list_append(options, model)) {
        return;
    }
}

DutyCalculatorVatOptionModelList commodity_presenter_applicable_vat_options(const CommodityAggregate *aggregate)
{
    DutyCalculatorVatOptionModelList options = { 0 };
    if (aggregate == NULL) {
        return options;
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (!measure_vat(measure)) {
            continue;
        }

        const AdditionalCodeModel *additional_code = additional_code_for_measure(aggregate, measure);
        const MeasureComponentModel *component = first_component_for_measure(aggregate, measure);
        DutyCalculatorVatOptionModel option = { 0 };
        if (additional_code != NULL) {
            const char *code = additional_code->additional_code != NULL ? additional_code->additional_code : measure->additional_code_id;
            size_t key_len = strlen(measure->additional_code_type_id == NULL ? "" : measure->additional_code_type_id) +
                             strlen(code == NULL ? "" : code) + 1;
            option.key = malloc(key_len);
            if (option.key == NULL) {
                duty_calculator_vat_option_model_list_free(&options);
                return (DutyCalculatorVatOptionModelList){ 0 };
            }
            snprintf(option.key,
                     key_len,
                     "%s%s",
                     measure->additional_code_type_id == NULL ? "" : measure->additional_code_type_id,
                     code == NULL ? "" : code);
            option.description = copy_string(additional_code->description);
        } else {
            option.key = copy_string("VAT");
            if (!append_percent_description(&option,
                                            measure->measure_type_description,
                                            component == NULL ? NULL : component->duty_amount)) {
                duty_calculator_vat_option_model_free(&option);
                duty_calculator_vat_option_model_list_free(&options);
                return (DutyCalculatorVatOptionModelList){ 0 };
            }
        }

        if (option.key == NULL || option.description == NULL) {
            duty_calculator_vat_option_model_free(&option);
            duty_calculator_vat_option_model_list_free(&options);
            return (DutyCalculatorVatOptionModelList){ 0 };
        }
        replace_vat_option(&options, option);
    }

    return options;
}

static yyjson_doc *read_additional_codes_doc(void)
{
    const char *paths[] = {
        getenv("ADDITIONAL_CODES_PATH"),
        "../db/additional_codes.json",
        "db/additional_codes.json",
    };

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        if (paths[i] == NULL || paths[i][0] == '\0') {
            continue;
        }

        size_t size = 0;
        char *json = read_file(paths[i], &size);
        if (json == NULL) {
            continue;
        }
        yyjson_doc *doc = yyjson_read(json, size, 0);
        free(json);
        if (doc != NULL) {
            return doc;
        }
    }
    return NULL;
}

static char *json_string_dup(yyjson_val *object, const char *key)
{
    yyjson_val *value = object == NULL ? NULL : yyjson_obj_get(object, key);
    const char *string = value == NULL ? NULL : yyjson_get_str(value);
    return copy_string(string);
}

static yyjson_val *additional_code_heading(yyjson_val *root, const char *type_id)
{
    yyjson_val *headings = yyjson_obj_get(root, "headings");
    const char *type_name = additional_code_type_name(type_id);
    return yyjson_obj_get(headings, type_name);
}

static yyjson_val *additional_code_override(yyjson_val *root, const char *code)
{
    yyjson_val *overrides = yyjson_obj_get(root, "code_overrides");
    return yyjson_obj_get(overrides, code);
}

static DutyCalculatorAdditionalCodeGroupModel *additional_code_group_for_measure_type(
    DutyCalculatorAdditionalCodeGroupModelList *groups,
    const char *measure_type_id)
{
    if (groups == NULL || measure_type_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < groups->len; i++) {
        if (streq(groups->items[i].measure_type_id, measure_type_id)) {
            return &groups->items[i];
        }
    }
    return NULL;
}

static int add_additional_code_group(DutyCalculatorAdditionalCodeGroupModelList *groups,
                                     const MeasureModel *measure,
                                     const AdditionalCodeModel *additional_code,
                                     yyjson_val *additional_codes_root)
{
    DutyCalculatorAdditionalCodeGroupModel group = { 0 };
    yyjson_val *heading = additional_code_heading(additional_codes_root,
                                                  additional_code->additional_code_type_id);
    group.measure_type_id = copy_string(measure->measure_type_id);
    group.measure_type_description = copy_string(measure->measure_type_description);
    if (heading != NULL) {
        group.heading_overlay = json_string_dup(heading, "overlay");
        group.heading_hint = json_string_dup(heading, "hint");
    }
    if (group.measure_type_id == NULL ||
        group.measure_type_description == NULL ||
        (heading != NULL && (group.heading_overlay == NULL || group.heading_hint == NULL))) {
        duty_calculator_additional_code_group_model_free(&group);
        return 0;
    }

    return duty_calculator_additional_code_group_model_list_append(groups, group);
}

static int build_additional_code_annotation(DutyCalculatorAdditionalCodeAnnotationModel *annotation,
                                            const MeasureModel *measure,
                                            const AdditionalCodeModel *additional_code,
                                            yyjson_val *additional_codes_root)
{
    char *code = join_strings(additional_code->additional_code_type_id,
                              additional_code->additional_code);
    yyjson_val *override = additional_code_override(additional_codes_root, code);

    if (override != NULL && yyjson_is_obj(override) && yyjson_obj_size(override) > 0) {
        annotation->code = json_string_dup(override, "code");
        annotation->overlay = json_string_dup(override, "overlay");
        annotation->hint = json_string_dup(override, "hint");
    } else {
        annotation->code = copy_string(code);
        annotation->overlay = copy_string(additional_code->description);
        annotation->hint = copy_string("");
    }
    annotation->geographical_area_id = copy_string(measure->geographical_area_id);
    annotation->measure_sid = copy_string(measure->measure_sid);
    free(code);

    return annotation->code != NULL &&
           annotation->overlay != NULL &&
           annotation->hint != NULL &&
           annotation->geographical_area_id != NULL &&
           annotation->measure_sid != NULL;
}

static int append_additional_code_annotation(DutyCalculatorAdditionalCodeGroupModel *group,
                                             DutyCalculatorAdditionalCodeAnnotationModel annotation)
{
    return duty_calculator_additional_code_annotation_model_list_append(&group->additional_codes,
                                                                       annotation);
}

static int measure_sid_seen(const char **seen_sids, size_t seen_count, const char *measure_sid)
{
    for (size_t i = 0; i < seen_count; i++) {
        if (streq(seen_sids[i], measure_sid)) {
            return 1;
        }
    }
    return 0;
}

DutyCalculatorAdditionalCodeGroupModelList commodity_presenter_applicable_additional_codes(const CommodityAggregate *aggregate)
{
    DutyCalculatorAdditionalCodeGroupModelList groups = { 0 };
    if (aggregate == NULL) {
        return groups;
    }

    yyjson_doc *doc = read_additional_codes_doc();
    yyjson_val *root = doc == NULL ? NULL : yyjson_doc_get_root(doc);
    if (root == NULL) {
        return groups;
    }

    const char **seen_sids = calloc(aggregate->applicable_measures.len == 0 ? 1 : aggregate->applicable_measures.len,
                                    sizeof(*seen_sids));
    if (seen_sids == NULL) {
        yyjson_doc_free(doc);
        return groups;
    }
    size_t seen_count = 0;

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (!measure_model_import(measure) ||
            measure_sid_seen(seen_sids, seen_count, measure->measure_sid)) {
            continue;
        }
        seen_sids[seen_count++] = measure->measure_sid;

        const AdditionalCodeModel *additional_code = additional_code_for_measure(aggregate, measure);
        if (additional_code == NULL ||
            !additional_code_applicable_type(additional_code->additional_code_type_id)) {
            continue;
        }

        DutyCalculatorAdditionalCodeGroupModel *group =
            additional_code_group_for_measure_type(&groups, measure->measure_type_id);
        if (group == NULL) {
            if (!add_additional_code_group(&groups, measure, additional_code, root)) {
                duty_calculator_additional_code_group_model_list_free(&groups);
                free(seen_sids);
                yyjson_doc_free(doc);
                return (DutyCalculatorAdditionalCodeGroupModelList){ 0 };
            }
            group = &groups.items[groups.len - 1];
        }

        DutyCalculatorAdditionalCodeAnnotationModel annotation = { 0 };
        if (!build_additional_code_annotation(&annotation, measure, additional_code, root) ||
            !append_additional_code_annotation(group, annotation)) {
            duty_calculator_additional_code_annotation_model_free(&annotation);
            duty_calculator_additional_code_group_model_list_free(&groups);
            free(seen_sids);
            yyjson_doc_free(doc);
            return (DutyCalculatorAdditionalCodeGroupModelList){ 0 };
        }
    }

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (!measure_model_import(measure) || measure->additional_code_sid != NULL) {
            continue;
        }

        DutyCalculatorAdditionalCodeGroupModel *group =
            additional_code_group_for_measure_type(&groups, measure->measure_type_id);
        if (group == NULL) {
            continue;
        }

        DutyCalculatorAdditionalCodeAnnotationModel annotation = {
            .code = copy_string("none"),
            .overlay = copy_string("No additional code"),
            .hint = copy_string(""),
            .geographical_area_id = copy_string(measure->geographical_area_id),
            .measure_sid = copy_string(measure->measure_sid),
        };
        if (annotation.code == NULL ||
            annotation.overlay == NULL ||
            annotation.hint == NULL ||
            annotation.geographical_area_id == NULL ||
            annotation.measure_sid == NULL ||
            !append_additional_code_annotation(group, annotation)) {
            duty_calculator_additional_code_annotation_model_free(&annotation);
            duty_calculator_additional_code_group_model_list_free(&groups);
            free(seen_sids);
            yyjson_doc_free(doc);
            return (DutyCalculatorAdditionalCodeGroupModelList){ 0 };
        }
    }

    free(seen_sids);
    yyjson_doc_free(doc);
    return groups;
}

static int measure_unit_expressable(const MeasureModel *measure)
{
    return measure_model_import(measure) &&
           (streq(measure->measure_type_series_id, "C") ||
            streq(measure->measure_type_series_id, "D") ||
            streq(measure->measure_type_series_id, "J") ||
            streq(measure->measure_type_series_id, "Q"));
}

static const MeasurementUnitModel *measurement_unit_for_code(const CommodityAggregate *aggregate,
                                                             const char *code)
{
    if (aggregate == NULL || code == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < aggregate->measurement_units.len; i++) {
        const MeasurementUnitModel *unit = &aggregate->measurement_units.items[i];
        if (streq(unit->measurement_unit_code, code)) {
            return unit;
        }
    }
    return NULL;
}

static char *unit_key_for(const char *measurement_unit_code,
                          const char *measurement_unit_qualifier_code)
{
    return join_strings(measurement_unit_code, measurement_unit_qualifier_code);
}

static char *read_file(const char *path, size_t *size_out)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }
    rewind(file);

    char *buffer = malloc((size_t)size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    size_t read = fread(buffer, 1, (size_t)size, file);
    fclose(file);
    if (read != (size_t)size) {
        free(buffer);
        return NULL;
    }

    buffer[read] = '\0';
    if (size_out != NULL) {
        *size_out = read;
    }
    return buffer;
}

static yyjson_doc *read_measurement_units_doc(void)
{
    const char *paths[] = {
        getenv("MEASUREMENT_UNITS_PATH"),
        "../db/measurement_units.json",
        "db/measurement_units.json",
    };

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        if (paths[i] == NULL || paths[i][0] == '\0') {
            continue;
        }

        size_t size = 0;
        char *json = read_file(paths[i], &size);
        if (json == NULL) {
            continue;
        }
        yyjson_doc *doc = yyjson_read(json, size, 0);
        free(json);
        if (doc != NULL) {
            return doc;
        }
    }
    return NULL;
}

static int copy_json_field(DutyCalculatorMetadataField *field,
                           yyjson_val *object,
                           const char *name)
{
    yyjson_val *value = yyjson_obj_get(object, name);
    if (value == NULL) {
        return 1;
    }

    field->present = 1;
    if (yyjson_is_null(value)) {
        field->is_null = 1;
        return 1;
    }

    const char *string = yyjson_get_str(value);
    if (string == NULL) {
        return 0;
    }
    field->value = copy_string(string);
    return field->value != NULL;
}

static int set_string_field(DutyCalculatorMetadataField *field,
                            const char *value)
{
    field->present = 1;
    field->is_null = 0;
    field->value = copy_string(value == NULL ? "" : value);
    return field->value != NULL;
}

static int set_null_field(DutyCalculatorMetadataField *field)
{
    field->present = 1;
    field->is_null = 1;
    return 1;
}

static int set_nullable_string_field(DutyCalculatorMetadataField *field,
                                     const char *value)
{
    if (value == NULL) {
        return set_null_field(field);
    }
    return set_string_field(field, value);
}

static int build_overlay_measure_unit(DutyCalculatorMeasureUnitModel *model,
                                      const char *key,
                                      yyjson_val *unit)
{
    model->key = copy_string(key);
    return model->key != NULL &&
           copy_json_field(&model->measurement_unit_code, unit, "measurement_unit_code") &&
           copy_json_field(&model->measurement_unit_qualifier_code, unit, "measurement_unit_qualifier_code") &&
           copy_json_field(&model->abbreviation, unit, "abbreviation") &&
           copy_json_field(&model->expansion, unit, "expansion") &&
           copy_json_field(&model->unit_question, unit, "unit_question") &&
           copy_json_field(&model->unit_hint, unit, "unit_hint") &&
           copy_json_field(&model->unit, unit, "unit") &&
           copy_json_field(&model->multiplier, unit, "multiplier") &&
           copy_json_field(&model->coerced_measurement_unit_code, unit, "coerced_measurement_unit_code") &&
           copy_json_field(&model->original_unit, unit, "original_unit") &&
           copy_json_field(&model->measurement_unit_type, unit, "measurement_unit_type");
}

static int build_fallback_measure_unit(DutyCalculatorMeasureUnitModel *model,
                                       const CommodityAggregate *aggregate,
                                       const char *unit_code,
                                       const char *unit_key)
{
    const MeasurementUnitModel *unit = measurement_unit_for_code(aggregate, unit_code);
    const char *qualifier_suffix = strlen(unit_key) > 3 ? unit_key + 3 : "";
    const char *code = unit != NULL && unit->measurement_unit_code != NULL ? unit->measurement_unit_code : unit_code;
    const char *description = unit != NULL && unit->description != NULL ? unit->description : unit_code;
    size_t question_len = strlen("Please enter unit: ") + strlen(description) + 1;
    size_t hint_len = strlen("Please correctly enter unit: ") + strlen(description) + 1;

    model->key = copy_string(unit_key);
    if (model->key == NULL ||
        !set_string_field(&model->measurement_unit_code, code) ||
        !set_nullable_string_field(&model->abbreviation, unit == NULL ? NULL : unit->abbreviation) ||
        !set_null_field(&model->unit)) {
        return 0;
    }

    if (qualifier_suffix[0] == '\0') {
        if (!set_null_field(&model->measurement_unit_qualifier_code)) {
            return 0;
        }
    } else if (!set_string_field(&model->measurement_unit_qualifier_code, qualifier_suffix)) {
        return 0;
    }

    model->unit_question.present = 1;
    model->unit_question.value = malloc(question_len);
    model->unit_hint.present = 1;
    model->unit_hint.value = malloc(hint_len);
    if (model->unit_question.value == NULL || model->unit_hint.value == NULL) {
        return 0;
    }
    snprintf(model->unit_question.value, question_len, "Please enter unit: %s", description);
    snprintf(model->unit_hint.value, hint_len, "Please correctly enter unit: %s", description);
    return 1;
}

static void replace_measure_unit(DutyCalculatorMeasureUnitModelList *units,
                                 DutyCalculatorMeasureUnitModel model)
{
    for (size_t i = 0; i < units->len; i++) {
        if (streq(units->items[i].key, model.key)) {
            duty_calculator_measure_unit_model_free(&units->items[i]);
            units->items[i] = model;
            return;
        }
    }

    if (!duty_calculator_measure_unit_model_list_append(units, model)) {
        return;
    }
}

static int append_measure_unit_from_key(DutyCalculatorMeasureUnitModelList *units,
                                        const CommodityAggregate *aggregate,
                                        yyjson_val *overlay,
                                        const char *unit_code,
                                        const char *unit_key)
{
    yyjson_val *unit = overlay == NULL ? NULL : yyjson_obj_get(overlay, unit_key);
    if (unit != NULL) {
        yyjson_val *compound_units = yyjson_obj_get(unit, "compound_units");
        if (compound_units != NULL && yyjson_is_arr(compound_units)) {
            const char *compound_unit_code = yyjson_get_str(yyjson_obj_get(unit, "measurement_unit_code"));
            size_t idx;
            size_t max;
            yyjson_val *compound_key;
            yyjson_arr_foreach(compound_units, idx, max, compound_key) {
                const char *child_key = yyjson_get_str(compound_key);
                if (child_key == NULL ||
                    !append_measure_unit_from_key(units,
                                                  aggregate,
                                                  overlay,
                                                  compound_unit_code == NULL ? unit_code : compound_unit_code,
                                                  child_key)) {
                    return 0;
                }
            }
            return 1;
        }

        DutyCalculatorMeasureUnitModel model = { 0 };
        if (!build_overlay_measure_unit(&model, unit_key, unit)) {
            duty_calculator_measure_unit_model_free(&model);
            return 0;
        }
        replace_measure_unit(units, model);
        return 1;
    }

    DutyCalculatorMeasureUnitModel model = { 0 };
    if (!build_fallback_measure_unit(&model, aggregate, unit_code, unit_key)) {
        duty_calculator_measure_unit_model_free(&model);
        return 0;
    }
    replace_measure_unit(units, model);
    return 1;
}

DutyCalculatorMeasureUnitModelList commodity_presenter_applicable_measure_units(const CommodityAggregate *aggregate)
{
    DutyCalculatorMeasureUnitModelList units = { 0 };
    if (aggregate == NULL) {
        return units;
    }

    yyjson_doc *doc = read_measurement_units_doc();
    yyjson_val *overlay = doc == NULL ? NULL : yyjson_doc_get_root(doc);

    for (size_t i = 0; i < aggregate->applicable_measures.len; i++) {
        const MeasureModel *measure = &aggregate->applicable_measures.items[i];
        if (!measure_unit_expressable(measure)) {
            continue;
        }

        for (size_t j = 0; j < aggregate->measure_components.len; j++) {
            const MeasureComponentModel *component = &aggregate->measure_components.items[j];
            if (!streq(component->measure_sid, measure->measure_sid) ||
                component->measurement_unit_code == NULL) {
                continue;
            }

            char *unit_key = unit_key_for(component->measurement_unit_code,
                                          component->measurement_unit_qualifier_code);
            if (unit_key == NULL ||
                !append_measure_unit_from_key(&units,
                                              aggregate,
                                              overlay,
                                              component->measurement_unit_code,
                                              unit_key)) {
                free(unit_key);
                yyjson_doc_free(doc);
                duty_calculator_measure_unit_model_list_free(&units);
                return (DutyCalculatorMeasureUnitModelList){ 0 };
            }
            free(unit_key);
        }
    }

    yyjson_doc_free(doc);
    return units;
}

int commodity_presenter_self_test(void)
{
    CommodityModel model = { .goods_nomenclature_sid = "123" };
    GoodsNomenclatureTreeNode node = { .goods_nomenclature_sid = "456" };
    MeasureModel measures[] = {
        {
            .measure_sid = "1",
            .measure_type_id = "103",
            .geographical_area_id = "1011",
            .trade_movement_code = "0",
        },
        {
            .measure_sid = "2",
            .measure_type_id = "672",
            .trade_movement_code = "0",
        },
    };
    MeasureComponentModel components[] = {
        {
            .measure_sid = "1",
            .duty_expression_id = "01",
            .duty_amount = "8.5",
        },
        {
            .measure_sid = "2",
            .duty_expression_id = "12",
            .duty_amount = "0",
            .duty_expression_description = "Agricultural component",
        },
    };
    CommodityAggregate aggregate = {
        .applicable_measures = {
            .items = measures,
            .len = 2,
            .cap = 2,
        },
        .measure_components = {
            .items = components,
            .len = 2,
            .cap = 2,
        },
    };
    char *basic_duty_rate = commodity_presenter_basic_duty_rate(&aggregate);
    int ok = strcmp(commodity_presenter_resource_type(), "commodity") == 0 &&
             strcmp(commodity_presenter_id(&model), "123") == 0 &&
             strcmp(commodity_presenter_tree_node_id(&node), "456") == 0 &&
             basic_duty_rate != NULL &&
             strcmp(basic_duty_rate, "8.5 %") == 0 &&
             commodity_presenter_meursing_code(&aggregate) &&
             !commodity_presenter_zero_mfn_duty(&aggregate) &&
             !commodity_presenter_trade_defence(&aggregate) &&
             !commodity_presenter_entry_price_system(&aggregate, "uk") &&
             duty_calculator_metadata_model_self_test();
    free(basic_duty_rate);
    return ok;
}
