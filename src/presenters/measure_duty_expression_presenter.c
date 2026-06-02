#include "presenters/measure_duty_expression_presenter.h"

#include "models/duty_expression_description_model.h"
#include "presenters/duty_expression_description_presenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
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

static char *format_amount(const char *amount)
{
    if (amount == NULL) {
        return NULL;
    }

    double value = strtod(amount, NULL);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.6f", value);

    char *end = buffer + strlen(buffer);
    while (end > buffer && end[-1] == '0') {
        *--end = '\0';
    }
    if (end > buffer && end[-1] == '.') {
        *--end = '\0';
    }
    return copy_literal(buffer);
}

static char *format_amount_fixed(const char *amount, int formatted)
{
    if (amount == NULL) {
        return NULL;
    }

    double value = strtod(amount, NULL);
    char buffer[96];
    if (formatted) {
        snprintf(buffer, sizeof(buffer), "<span>%.2f</span>", value);
    } else {
        snprintf(buffer, sizeof(buffer), "%.2f", value);
    }
    return copy_literal(buffer);
}

static int append_fragment(char **buffer, size_t *len, size_t *cap, const char *fragment)
{
    if (buffer == NULL || len == NULL || cap == NULL || fragment == NULL || *fragment == '\0') {
        return 1;
    }

    size_t fragment_len = strlen(fragment);
    size_t extra = fragment_len + (*len == 0 ? 0 : 1);
    if (*len + extra + 1 > *cap) {
        size_t next_cap = *cap == 0 ? 64 : *cap;
        while (*len + extra + 1 > next_cap) {
            next_cap *= 2;
        }
        char *next = realloc(*buffer, next_cap);
        if (next == NULL) {
            return 0;
        }
        *buffer = next;
        *cap = next_cap;
    }

    if (*len > 0) {
        (*buffer)[(*len)++] = ' ';
    }
    memcpy(*buffer + *len, fragment, fragment_len);
    *len += fragment_len;
    (*buffer)[*len] = '\0';
    return 1;
}

static int description_only_expression(const char *id)
{
    static const char *ids[] = { "12", "14", "37", "40", "41", "42", "43", "44", "21", "25", "27", "29" };
    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
        if (streq(id, ids[i])) {
            return 1;
        }
    }
    return 0;
}

static int description_amount_expression(const char *id)
{
    static const char *ids[] = { "02", "04", "15", "17", "19", "20", "36" };
    for (size_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
        if (streq(id, ids[i])) {
            return 1;
        }
    }
    return 0;
}

typedef enum {
    DUTY_EXPRESSION_LEGACY,
    DUTY_EXPRESSION_BASE,
    DUTY_EXPRESSION_FORMATTED,
    DUTY_EXPRESSION_VERBOSE,
} DutyExpressionRenderMode;

static int append_measurement_unit(char **buffer,
                                   size_t *len,
                                   size_t *cap,
                                   const MeasureComponentModel *component)
{
    if (component->measurement_unit_code == NULL) {
        return 1;
    }
    return append_fragment(buffer, len, cap, "/") &&
           append_fragment(buffer, len, cap, component->measurement_unit_code);
}

static char *normalize_verbose(char *value)
{
    if (value == NULL) {
        return NULL;
    }

    char *read = value;
    char *write = value;
    while (*read != '\0') {
        if (*read == ' ' && read[1] == ' ') {
            read++;
            continue;
        }
        if (*read == ' ' && read[1] == '%' && write > value && write[-1] >= '0' && write[-1] <= '9') {
            read++;
            continue;
        }
        *write++ = *read++;
    }
    *write = '\0';
    return value;
}

static char *format_component_for_mode(const MeasureComponentModel *component,
                                       DutyExpressionRenderMode mode)
{
    if (component == NULL) {
        return NULL;
    }

    DutyExpressionDescriptionModel description = {
        .duty_expression_id = component->duty_expression_id,
        .description = component->duty_expression_description,
    };
    const char *abbreviation = duty_expression_description_presenter_abbreviation(&description);
    const char *text = abbreviation != NULL ? abbreviation : component->duty_expression_description;
    char *amount = mode == DUTY_EXPRESSION_LEGACY
                       ? format_amount(component->duty_amount)
                       : format_amount_fixed(component->duty_amount, mode == DUTY_EXPRESSION_FORMATTED);
    char *buffer = NULL;
    size_t len = 0;
    size_t cap = 0;
    int ok = 1;

    if (streq(component->duty_expression_id, "99")) {
        ok = append_fragment(&buffer, &len, &cap, component->measurement_unit_code);
    } else if (description_only_expression(component->duty_expression_id)) {
        ok = append_fragment(&buffer, &len, &cap, text);
    } else if (description_amount_expression(component->duty_expression_id)) {
        ok = append_fragment(&buffer, &len, &cap, text) &&
             append_fragment(&buffer, &len, &cap, amount) &&
             append_fragment(&buffer, &len, &cap, component->monetary_unit_code != NULL ? component->monetary_unit_code : "%") &&
             append_measurement_unit(&buffer, &len, &cap, component);
    } else {
        ok = append_fragment(&buffer, &len, &cap, amount);
        if (mode == DUTY_EXPRESSION_LEGACY) {
            ok = ok && append_fragment(&buffer, &len, &cap, component->monetary_unit_code != NULL ? component->monetary_unit_code : (text != NULL ? text : "%"));
        } else {
            ok = ok &&
                 append_fragment(&buffer, &len, &cap, text != NULL && component->monetary_unit_code == NULL ? text : "%") &&
                 append_fragment(&buffer, &len, &cap, component->monetary_unit_code) &&
                 append_measurement_unit(&buffer, &len, &cap, component);
        }
    }

    free(amount);
    if (!ok) {
        free(buffer);
        return NULL;
    }
    if (buffer == NULL) {
        return copy_literal("");
    }
    return mode == DUTY_EXPRESSION_VERBOSE ? normalize_verbose(buffer) : buffer;
}

static char *format_measure_components(const MeasureModel *measure,
                                       const MeasureComponentModelList *components,
                                       DutyExpressionRenderMode mode)
{
    if (measure == NULL || measure->measure_sid == NULL || components == NULL) {
        return NULL;
    }

    char *buffer = NULL;
    size_t len = 0;
    size_t cap = 0;
    for (size_t i = 0; i < components->len; i++) {
        const MeasureComponentModel *component = &components->items[i];
        const char *owner_measure_sid = component->resolved_for_measure_sid != NULL
                                            ? component->resolved_for_measure_sid
                                            : component->measure_sid;
        if (!streq(owner_measure_sid, measure->measure_sid)) {
            continue;
        }

        char *formatted = format_component_for_mode(component, mode);
        int ok = formatted != NULL && append_fragment(&buffer, &len, &cap, formatted);
        free(formatted);
        if (!ok) {
            free(buffer);
            return NULL;
        }
    }

    if (buffer == NULL) {
        return copy_literal("");
    }
    return mode == DUTY_EXPRESSION_VERBOSE ? normalize_verbose(buffer) : buffer;
}

char *measure_duty_expression_presenter_formatted(const MeasureModel *measure,
                                                  const MeasureComponentModelList *components)
{
    char *formatted = format_measure_components(measure, components, DUTY_EXPRESSION_LEGACY);
    if (formatted != NULL && formatted[0] == '\0') {
        free(formatted);
        return NULL;
    }
    return formatted;
}

char *measure_duty_expression_presenter_resolved(const MeasureModel *measure,
                                                 const MeasureComponentModelList *components)
{
    if (measure == NULL || measure->measure_sid == NULL) {
        return NULL;
    }

    return format_measure_components(measure, components, DUTY_EXPRESSION_FORMATTED);
}

char *measure_duty_expression_presenter_id(const MeasureModel *measure)
{
    if (measure == NULL || measure->measure_sid == NULL) {
        return NULL;
    }

    size_t id_len = strlen(measure->measure_sid) + strlen("-duty_expression") + 1;
    char *id = malloc(id_len);
    if (id == NULL) {
        return NULL;
    }
    snprintf(id, id_len, "%s-duty_expression", measure->measure_sid);
    return id;
}

DutyExpressionModel measure_duty_expression_presenter_build(const MeasureModel *measure,
                                                            const MeasureComponentModelList *components)
{
    DutyExpressionModel model = { 0 };
    if (measure == NULL || measure->measure_sid == NULL || components == NULL) {
        return model;
    }

    model.id = measure_duty_expression_presenter_id(measure);
    if (model.id == NULL) {
        return model;
    }

    model.base = format_measure_components(measure, components, DUTY_EXPRESSION_BASE);
    model.formatted_base = format_measure_components(measure, components, DUTY_EXPRESSION_FORMATTED);
    model.verbose_duty = format_measure_components(measure, components, DUTY_EXPRESSION_VERBOSE);

    if (model.base == NULL || model.formatted_base == NULL || model.verbose_duty == NULL) {
        duty_expression_model_free(&model);
    }
    return model;
}

int measure_duty_expression_presenter_self_test(void)
{
    MeasureModel measure = {
        .measure_sid = "1",
    };
    MeasureComponentModel components_items[] = {
        {
            .measure_sid = "1",
            .duty_expression_id = "01",
            .duty_amount = "8.500000",
        },
    };
    MeasureComponentModelList components = {
        .items = components_items,
        .len = 1,
        .cap = 1,
    };
    MeasureComponentModel resolved_items[] = {
        {
            .measure_sid = "900",
            .duty_expression_id = "04",
            .duty_amount = "100.000000",
            .monetary_unit_code = "EUR",
            .measurement_unit_code = "DTN",
            .resolved_for_measure_sid = "1",
            .resolved_meursing = 1,
        },
    };
    MeasureComponentModelList resolved_components = {
        .items = resolved_items,
        .len = 1,
        .cap = 1,
    };
    char *formatted = measure_duty_expression_presenter_formatted(&measure, &components);
    char *unresolved = measure_duty_expression_presenter_resolved(&measure, &(MeasureComponentModelList){ 0 });
    char *resolved = measure_duty_expression_presenter_resolved(&measure, &resolved_components);
    char *id = measure_duty_expression_presenter_id(&measure);
    DutyExpressionModel expression = measure_duty_expression_presenter_build(&measure, &components);
    int ok = formatted != NULL && strcmp(formatted, "8.5 %") == 0 &&
             unresolved != NULL && strcmp(unresolved, "") == 0 &&
             resolved != NULL && strcmp(resolved, "+ <span>100.00</span> EUR / DTN") == 0 &&
             id != NULL && strcmp(id, "1-duty_expression") == 0 &&
             expression.id != NULL && strcmp(expression.id, "1-duty_expression") == 0 &&
             expression.base != NULL && strcmp(expression.base, "8.50 %") == 0 &&
             expression.formatted_base != NULL && strcmp(expression.formatted_base, "<span>8.50</span> %") == 0 &&
             expression.verbose_duty != NULL && strcmp(expression.verbose_duty, "8.50%") == 0;
    free(formatted);
    free(unresolved);
    free(resolved);
    free(id);
    duty_expression_model_free(&expression);
    return ok;
}
