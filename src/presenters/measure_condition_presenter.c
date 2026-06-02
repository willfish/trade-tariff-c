#include "presenters/measure_condition_presenter.h"

#include "presenters/measure_condition_component_presenter.h"
#include "presenters/monetary_unit_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int blank(const char *value)
{
    return value == NULL || *value == '\0';
}

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static char *copy_literal(const char *value)
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

const char *measure_condition_presenter_resource_type(void)
{
    return "measure_condition";
}

const char *measure_condition_presenter_id(const MeasureConditionModel *model)
{
    return model == NULL ? NULL : model->measure_condition_sid;
}

char *measure_condition_presenter_document_code(const MeasureConditionModel *model)
{
    if (model == NULL || blank(model->certificate_type_code) || blank(model->certificate_code)) {
        return NULL;
    }

    size_t len = strlen(model->certificate_type_code) + strlen(model->certificate_code) + 1;
    char *document_code = malloc(len);
    if (document_code == NULL) {
        return NULL;
    }
    snprintf(document_code, len, "%s%s", model->certificate_type_code, model->certificate_code);
    return document_code;
}

char *measure_condition_presenter_condition(const MeasureConditionModel *model)
{
    if (model == NULL || blank(model->condition_code)) {
        return NULL;
    }

    const char *description = model->measure_condition_code_description == NULL
                                  ? ""
                                  : model->measure_condition_code_description;
    size_t len = strlen(model->condition_code) + strlen(": ") + strlen(description) + 1;
    char *condition = malloc(len);
    if (condition == NULL) {
        return NULL;
    }
    snprintf(condition, len, "%s: %s", model->condition_code, description);
    return condition;
}

static char *document_requirement(const MeasureConditionModel *model)
{
    const char *type_description = model->certificate_type_description == NULL ? "" : model->certificate_type_description;
    const char *certificate_description = model->certificate_description == NULL ? "" : model->certificate_description;
    size_t len = strlen(type_description) + strlen(": ") + strlen(certificate_description) + 1;
    char *requirement = malloc(len);
    if (requirement == NULL) {
        return NULL;
    }
    snprintf(requirement, len, "%s: %s", type_description, certificate_description);
    return requirement;
}

static char *formatted_amount(const char *amount)
{
    if (blank(amount)) {
        return NULL;
    }

    size_t len = strlen("<span></span>") + strlen(amount) + 1;
    char *formatted = malloc(len);
    if (formatted == NULL) {
        return NULL;
    }
    snprintf(formatted, len, "<span>%s</span>", amount);
    return formatted;
}

static char *measurement_fragment(const char *measurement_unit)
{
    if (blank(measurement_unit)) {
        return NULL;
    }

    size_t len = strlen("<abbr title=''></abbr>") + strlen(measurement_unit) * 2 + 1;
    char *fragment = malloc(len);
    if (fragment == NULL) {
        return NULL;
    }
    snprintf(fragment, len, "<abbr title='%s'>%s</abbr>", measurement_unit, measurement_unit);
    return fragment;
}

static char *duty_requirement(const MeasureConditionModel *model)
{
    char *amount = formatted_amount(model->condition_duty_amount);
    char *measurement = measurement_fragment(model->measurement_unit_description);
    const char *abbreviation = measure_condition_presenter_monetary_unit_abbreviation(model);
    const char *monetary = blank(abbreviation) ? model->condition_monetary_unit_code : abbreviation;

    size_t len = 1;
    if (amount != NULL) {
        len += strlen(amount);
    }
    if (!blank(monetary)) {
        len += (len > 1 ? 1 : 0) + strlen(monetary);
    }
    if (measurement != NULL) {
        len += (len > 1 ? strlen(" / ") : 0) + strlen(measurement);
    }

    char *requirement = malloc(len);
    if (requirement == NULL) {
        free(amount);
        free(measurement);
        return NULL;
    }
    requirement[0] = '\0';

    if (amount != NULL) {
        strcat(requirement, amount);
    }
    if (!blank(monetary)) {
        if (requirement[0] != '\0') {
            strcat(requirement, " ");
        }
        strcat(requirement, monetary);
    }
    if (measurement != NULL) {
        if (requirement[0] != '\0') {
            strcat(requirement, " / ");
        }
        strcat(requirement, measurement);
    }

    free(amount);
    free(measurement);
    return requirement[0] == '\0' ? (free(requirement), NULL) : requirement;
}

char *measure_condition_presenter_requirement(const MeasureConditionModel *model)
{
    if (model == NULL) {
        return NULL;
    }
    if (!blank(model->certificate_code)) {
        return document_requirement(model);
    }
    if (!blank(model->condition_duty_amount)) {
        return duty_requirement(model);
    }
    return NULL;
}

static char *formatted_component_duty_expression(const MeasureConditionComponentModel *component)
{
    if (component == NULL) {
        return NULL;
    }

    char *amount = formatted_amount(component->duty_amount);
    const char *abbreviation = measure_condition_component_presenter_duty_expression_abbreviation(component);
    const char *monetary_abbreviation = monetary_unit_presenter_abbreviation_for_code(component->monetary_unit_code);
    const char *monetary = blank(monetary_abbreviation) ? component->monetary_unit_code : monetary_abbreviation;
    const char *measurement = component->measurement_unit_code;
    const char *qualifier = component->measurement_unit_qualifier_code;

    size_t len = 1;
    if (amount != NULL) {
        len += strlen(amount);
    }
    if (!blank(abbreviation)) {
        len += (len > 1 ? 1 : 0) + strlen(abbreviation);
    }
    if (!blank(monetary)) {
        len += (len > 1 ? 1 : 0) + strlen(monetary);
    }
    if (!blank(measurement)) {
        len += strlen(" / ") + strlen(measurement);
    }
    if (!blank(qualifier)) {
        len += 1 + strlen(qualifier);
    }

    char *formatted = malloc(len);
    if (formatted == NULL) {
        free(amount);
        return NULL;
    }
    formatted[0] = '\0';

    if (amount != NULL) {
        strcat(formatted, amount);
    }
    if (!blank(abbreviation)) {
        if (formatted[0] != '\0') {
            strcat(formatted, " ");
        }
        strcat(formatted, abbreviation);
    }
    if (!blank(monetary)) {
        if (formatted[0] != '\0') {
            strcat(formatted, " ");
        }
        strcat(formatted, monetary);
    }
    if (!blank(measurement)) {
        strcat(formatted, " / ");
        strcat(formatted, measurement);
        if (!blank(qualifier)) {
            strcat(formatted, " ");
            strcat(formatted, qualifier);
        }
    }

    free(amount);
    return formatted[0] == '\0' ? (free(formatted), NULL) : formatted;
}

char *measure_condition_presenter_duty_expression(const MeasureConditionModel *model,
                                                  const MeasureConditionComponentModelList *components)
{
    if (model == NULL || model->measure_condition_sid == NULL || components == NULL) {
        return copy_literal("");
    }

    char *joined = copy_literal("");
    if (joined == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < components->len; i++) {
        const MeasureConditionComponentModel *component = &components->items[i];
        if (!streq(component->measure_condition_sid, model->measure_condition_sid)) {
            continue;
        }

        char *fragment = formatted_component_duty_expression(component);
        if (fragment == NULL) {
            continue;
        }

        size_t joined_len = strlen(joined);
        size_t fragment_len = strlen(fragment);
        char *next = realloc(joined, joined_len + (joined_len > 0 ? 1 : 0) + fragment_len + 1);
        if (next == NULL) {
            free(joined);
            free(fragment);
            return NULL;
        }
        joined = next;
        if (joined_len > 0) {
            joined[joined_len++] = ' ';
        }
        memcpy(joined + joined_len, fragment, fragment_len + 1);
        free(fragment);
    }

    return joined;
}

const char *measure_condition_presenter_monetary_unit_abbreviation(const MeasureConditionModel *model)
{
    if (model == NULL) {
        return NULL;
    }
    return monetary_unit_presenter_abbreviation_for_code(model->condition_monetary_unit_code);
}

const char *measure_condition_presenter_guidance_cds(const MeasureConditionModel *model,
                                                     const Appendix5aModelList *appendix_5as)
{
    if (model == NULL ||
        appendix_5as == NULL ||
        blank(model->certificate_type_code) ||
        blank(model->certificate_code)) {
        return NULL;
    }

    for (size_t i = 0; i < appendix_5as->len; i++) {
        const Appendix5aModel *appendix_5a = &appendix_5as->items[i];
        if (streq(appendix_5a->certificate_type_code, model->certificate_type_code) &&
            streq(appendix_5a->certificate_code, model->certificate_code)) {
            return appendix_5a->cds_guidance;
        }
    }

    return NULL;
}

static int negative_action(const char *action_code)
{
    static const char *codes[] = { "04", "05", "06", "08", "09", "16" };
    for (size_t i = 0; i < sizeof(codes) / sizeof(codes[0]); i++) {
        if (streq(action_code, codes[i])) {
            return 1;
        }
    }
    return 0;
}

const char *measure_condition_presenter_measure_condition_class(const MeasureConditionModel *model)
{
    if (model == NULL) {
        return NULL;
    }
    if (!blank(model->condition_duty_amount)) {
        return "threshold";
    }
    if (negative_action(model->action_code)) {
        return "negative";
    }

    char *doc_code = measure_condition_presenter_document_code(model);
    const char *klass = "unknown";
    if (doc_code != NULL && streq(model->certificate_type_code, "Y")) {
        klass = "exemption";
    } else if (doc_code != NULL && !streq(doc_code, "999L") && !streq(doc_code, "C084")) {
        klass = "document";
    }
    free(doc_code);
    return klass;
}

const char *measure_condition_presenter_requirement_operator(const MeasureConditionModel *model)
{
    if (model == NULL || model->condition_code == NULL) {
        return NULL;
    }

    typedef struct {
        const char *code;
        const char *operator;
    } OperatorMapping;
    static const OperatorMapping mappings[] = {
        { "E", "=<" }, { "F", "=>" }, { "G", "=>" }, { "I", "=<" }, { "J", ">" },
        { "L", ">" },  { "M", "=>" }, { "N", "=>" }, { "O", ">" },  { "R", "=>" },
        { "U", ">" },  { "V", "=>" }, { "X", ">" },
    };

    for (size_t i = 0; i < sizeof(mappings) / sizeof(mappings[0]); i++) {
        if (streq(model->condition_code, mappings[i].code)) {
            return mappings[i].operator;
        }
    }
    return NULL;
}

static int in_list(const char *value, const char *const *items, size_t count)
{
    if (value == NULL) {
        return 0;
    }
    for (size_t i = 0; i < count; i++) {
        if (streq(value, items[i])) {
            return 1;
        }
    }
    return 0;
}

const char *measure_condition_presenter_threshold_unit_type(const MeasureConditionModel *model)
{
    if (model == NULL || blank(model->condition_duty_amount)) {
        return NULL;
    }

    static const char *weight_units[] = {
        "CCT", "CTM", "DAP", "DHS", "DTN", "DTNE", "DTNF", "DTNG", "DTNL", "DTNM", "DTNR", "DTNS",
        "GFI", "GRM", "GRT", "KCC", "KCL", "KGM", "KGMA", "KGME", "KGMG", "KGMP", "KGMS", "KGMT",
        "KMA", "KNI", "KNS", "KPH", "KPO", "KPP", "KSD", "KSH", "KUR", "TNE", "TNEE", "TNEI",
        "TNEJ", "TNEK", "TNEM", "TNER", "TNEZ",
    };
    static const char *volume_units[] = { "HLT", "KLT", "LPA", "LTR", "LTRA", "MTQ", "MTQC" };

    int price = !blank(model->condition_monetary_unit_code);
    int weight = in_list(model->condition_measurement_unit_code, weight_units, sizeof(weight_units) / sizeof(weight_units[0]));
    int volume = in_list(model->condition_measurement_unit_code, volume_units, sizeof(volume_units) / sizeof(volume_units[0]));
    int percentage_abv = streq(model->condition_measurement_unit_code, "ASV");

    if (streq(model->condition_code, "V") && price && weight) {
        return "eps";
    }
    if (price) {
        return "price";
    }
    if (weight) {
        return "weight";
    }
    if (volume) {
        return "volume";
    }
    if (percentage_abv) {
        return "percentage_abv";
    }
    return NULL;
}

int measure_condition_presenter_self_test(void)
{
    MeasureConditionModel model = {
        .measure_condition_sid = "789",
        .condition_code = "X",
        .condition_duty_amount = "10",
        .condition_measurement_unit_code = "KGM",
        .certificate_type_code = "C",
        .certificate_code = "990",
        .measure_condition_code_description = "Condition text",
    };
    char *condition = measure_condition_presenter_condition(&model);
    char *document_code = measure_condition_presenter_document_code(&model);
    int ok = strcmp(measure_condition_presenter_resource_type(), "measure_condition") == 0 &&
             strcmp(measure_condition_presenter_id(&model), "789") == 0 &&
             condition != NULL &&
             strcmp(condition, "X: Condition text") == 0 &&
             document_code != NULL &&
             strcmp(document_code, "C990") == 0 &&
             strcmp(measure_condition_presenter_requirement_operator(&model), ">") == 0 &&
             strcmp(measure_condition_presenter_measure_condition_class(&model), "threshold") == 0 &&
             strcmp(measure_condition_presenter_threshold_unit_type(&model), "weight") == 0 &&
             measure_condition_presenter_guidance_cds(&model, NULL) == NULL;
    free(condition);
    free(document_code);
    return ok;
}
