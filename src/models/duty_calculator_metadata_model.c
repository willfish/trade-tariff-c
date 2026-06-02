#include "models/duty_calculator_metadata_model.h"

#include <stdlib.h>
#include <string.h>

void duty_calculator_additional_code_annotation_model_free(DutyCalculatorAdditionalCodeAnnotationModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->code);
    free(model->overlay);
    free(model->hint);
    free(model->geographical_area_id);
    free(model->measure_sid);
    *model = (DutyCalculatorAdditionalCodeAnnotationModel){ 0 };
}

void duty_calculator_additional_code_annotation_model_list_free(DutyCalculatorAdditionalCodeAnnotationModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        duty_calculator_additional_code_annotation_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (DutyCalculatorAdditionalCodeAnnotationModelList){ 0 };
}

int duty_calculator_additional_code_annotation_model_list_append(DutyCalculatorAdditionalCodeAnnotationModelList *list,
                                                                 DutyCalculatorAdditionalCodeAnnotationModel model)
{
    if (list == NULL) {
        duty_calculator_additional_code_annotation_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        DutyCalculatorAdditionalCodeAnnotationModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            duty_calculator_additional_code_annotation_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = model;
    return 1;
}

void duty_calculator_additional_code_group_model_free(DutyCalculatorAdditionalCodeGroupModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measure_type_id);
    free(model->measure_type_description);
    free(model->heading_overlay);
    free(model->heading_hint);
    duty_calculator_additional_code_annotation_model_list_free(&model->additional_codes);
    *model = (DutyCalculatorAdditionalCodeGroupModel){ 0 };
}

void duty_calculator_additional_code_group_model_list_free(DutyCalculatorAdditionalCodeGroupModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        duty_calculator_additional_code_group_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (DutyCalculatorAdditionalCodeGroupModelList){ 0 };
}

int duty_calculator_additional_code_group_model_list_append(DutyCalculatorAdditionalCodeGroupModelList *list,
                                                            DutyCalculatorAdditionalCodeGroupModel model)
{
    if (list == NULL) {
        duty_calculator_additional_code_group_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        DutyCalculatorAdditionalCodeGroupModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            duty_calculator_additional_code_group_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = model;
    return 1;
}

void duty_calculator_metadata_field_free(DutyCalculatorMetadataField *field)
{
    if (field == NULL) {
        return;
    }

    free(field->value);
    *field = (DutyCalculatorMetadataField){ 0 };
}

void duty_calculator_vat_option_model_free(DutyCalculatorVatOptionModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->key);
    free(model->description);
    *model = (DutyCalculatorVatOptionModel){ 0 };
}

void duty_calculator_vat_option_model_list_free(DutyCalculatorVatOptionModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        duty_calculator_vat_option_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (DutyCalculatorVatOptionModelList){ 0 };
}

int duty_calculator_vat_option_model_list_append(DutyCalculatorVatOptionModelList *list,
                                                 DutyCalculatorVatOptionModel model)
{
    if (list == NULL) {
        duty_calculator_vat_option_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        DutyCalculatorVatOptionModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            duty_calculator_vat_option_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = model;
    return 1;
}

void duty_calculator_measure_unit_model_free(DutyCalculatorMeasureUnitModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->key);
    duty_calculator_metadata_field_free(&model->measurement_unit_code);
    duty_calculator_metadata_field_free(&model->measurement_unit_qualifier_code);
    duty_calculator_metadata_field_free(&model->abbreviation);
    duty_calculator_metadata_field_free(&model->expansion);
    duty_calculator_metadata_field_free(&model->unit_question);
    duty_calculator_metadata_field_free(&model->unit_hint);
    duty_calculator_metadata_field_free(&model->unit);
    duty_calculator_metadata_field_free(&model->multiplier);
    duty_calculator_metadata_field_free(&model->coerced_measurement_unit_code);
    duty_calculator_metadata_field_free(&model->original_unit);
    duty_calculator_metadata_field_free(&model->measurement_unit_type);
    *model = (DutyCalculatorMeasureUnitModel){ 0 };
}

void duty_calculator_measure_unit_model_list_free(DutyCalculatorMeasureUnitModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        duty_calculator_measure_unit_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (DutyCalculatorMeasureUnitModelList){ 0 };
}

int duty_calculator_measure_unit_model_list_append(DutyCalculatorMeasureUnitModelList *list,
                                                   DutyCalculatorMeasureUnitModel model)
{
    if (list == NULL) {
        duty_calculator_measure_unit_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        DutyCalculatorMeasureUnitModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            duty_calculator_measure_unit_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = model;
    return 1;
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

int duty_calculator_metadata_model_self_test(void)
{
    DutyCalculatorVatOptionModelList vat_options = { 0 };
    DutyCalculatorAdditionalCodeGroupModelList additional_code_groups = { 0 };
    DutyCalculatorVatOptionModel vat_option = {
        .key = copy_literal("VAT"),
        .description = copy_literal("Value added tax (20%)"),
    };
    DutyCalculatorAdditionalCodeGroupModel additional_code_group = {
        .measure_type_id = copy_literal("552"),
        .measure_type_description = copy_literal("Anti-dumping duty"),
        .heading_overlay = copy_literal("From which company are you buying these goods?"),
        .heading_hint = copy_literal("Additional duties are levied against imports from certain companies."),
    };
    DutyCalculatorAdditionalCodeAnnotationModel annotation = {
        .code = copy_literal("8123"),
        .overlay = copy_literal("Additional duty"),
        .hint = copy_literal(""),
        .geographical_area_id = copy_literal("CN"),
        .measure_sid = copy_literal("1"),
    };
    DutyCalculatorMeasureUnitModelList measure_units = { 0 };
    DutyCalculatorMeasureUnitModel measure_unit = {
        .key = copy_literal("KGM"),
        .measurement_unit_code = {
            .value = copy_literal("KGM"),
            .present = 1,
        },
        .measurement_unit_qualifier_code = {
            .present = 1,
            .is_null = 1,
        },
    };

    int ok = additional_code_group.measure_type_id != NULL &&
             additional_code_group.measure_type_description != NULL &&
             additional_code_group.heading_overlay != NULL &&
             additional_code_group.heading_hint != NULL &&
             annotation.code != NULL &&
             annotation.overlay != NULL &&
             annotation.hint != NULL &&
             annotation.geographical_area_id != NULL &&
             annotation.measure_sid != NULL &&
             duty_calculator_additional_code_annotation_model_list_append(&additional_code_group.additional_codes, annotation) &&
             duty_calculator_additional_code_group_model_list_append(&additional_code_groups, additional_code_group) &&
             vat_option.key != NULL &&
             vat_option.description != NULL &&
             measure_unit.key != NULL &&
             measure_unit.measurement_unit_code.value != NULL &&
             duty_calculator_vat_option_model_list_append(&vat_options, vat_option) &&
             duty_calculator_measure_unit_model_list_append(&measure_units, measure_unit) &&
             additional_code_groups.len == 1 &&
             additional_code_groups.items[0].additional_codes.len == 1 &&
             strcmp(additional_code_groups.items[0].additional_codes.items[0].code, "8123") == 0 &&
             vat_options.len == 1 &&
             strcmp(vat_options.items[0].key, "VAT") == 0 &&
             strcmp(vat_options.items[0].description, "Value added tax (20%)") == 0 &&
             measure_units.len == 1 &&
             strcmp(measure_units.items[0].key, "KGM") == 0 &&
             strcmp(measure_units.items[0].measurement_unit_code.value, "KGM") == 0 &&
             measure_units.items[0].measurement_unit_qualifier_code.present &&
             measure_units.items[0].measurement_unit_qualifier_code.is_null;

    duty_calculator_additional_code_group_model_list_free(&additional_code_groups);
    duty_calculator_vat_option_model_list_free(&vat_options);
    duty_calculator_measure_unit_model_list_free(&measure_units);
    return ok;
}
