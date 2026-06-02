#include "presenters/measurement_unit_presenter.h"

#include <stdlib.h>
#include <string.h>

static size_t count_occurrences(const char *input, const char *needle)
{
    size_t count = 0;
    size_t needle_len = strlen(needle);
    const char *cursor = input;
    while ((cursor = strstr(cursor, needle)) != NULL) {
        count++;
        cursor += needle_len;
    }
    return count;
}

static char *replace_all_owned(char *input, const char *needle, const char *replacement)
{
    size_t count = count_occurrences(input, needle);
    if (count == 0) {
        return input;
    }

    size_t input_len = strlen(input);
    size_t needle_len = strlen(needle);
    size_t replacement_len = strlen(replacement);
    char *output = malloc(input_len + count * (replacement_len - needle_len) + 1);
    if (output == NULL) {
        free(input);
        return NULL;
    }

    char *out = output;
    const char *cursor = input;
    const char *match;
    while ((match = strstr(cursor, needle)) != NULL) {
        size_t prefix_len = (size_t)(match - cursor);
        memcpy(out, cursor, prefix_len);
        out += prefix_len;
        memcpy(out, replacement, replacement_len);
        out += replacement_len;
        cursor = match + needle_len;
    }
    strcpy(out, cursor);
    free(input);
    return output;
}

static char *remove_spaces_before_percent_owned(char *input)
{
    char *output = malloc(strlen(input) + 1);
    if (output == NULL) {
        free(input);
        return NULL;
    }

    char *out = output;
    for (const char *in = input; *in != '\0'; in++) {
        if (*in == ' ') {
            const char *next = in;
            while (*next == ' ') {
                next++;
            }
            if (*next == '%' && out > output && out[-1] >= '0' && out[-1] <= '9') {
                continue;
            }
        }
        *out++ = *in;
    }
    *out = '\0';
    free(input);
    return output;
}

const char *measurement_unit_presenter_resource_type(void)
{
    return "measurement_unit";
}

const char *measurement_unit_presenter_id(const MeasurementUnitModel *model)
{
    return model == NULL ? NULL : model->measurement_unit_code;
}

char *measurement_unit_presenter_verbose_expansion_prefix(const char *expansion)
{
    const char *prefix = "number of ";
    if (expansion == NULL || expansion[0] == '\0') {
        return NULL;
    }

    size_t len = strlen(prefix) + strlen(expansion);
    char *value = malloc(len + 1);
    if (value == NULL) {
        return NULL;
    }
    strcpy(value, prefix);
    strcat(value, expansion);
    return value;
}

char *measurement_unit_presenter_normalize_verbose_owned(char *value)
{
    if (value == NULL) {
        return NULL;
    }
    value = replace_all_owned(value, "\xC2\xA0", " ");
    if (value == NULL) {
        return NULL;
    }
    value = replace_all_owned(value,
                              "carats (one metric carat = 2 x 10<sup>-4</sup>kg) (c/k)",
                              "carat (metric) (c/k)");
    if (value == NULL) {
        return NULL;
    }
    return remove_spaces_before_percent_owned(value);
}

int measurement_unit_presenter_self_test(void)
{
    MeasurementUnitModel model = { .measurement_unit_code = "KGM" };
    char *prefix = measurement_unit_presenter_verbose_expansion_prefix("pairs (pa)");
    char *normalized = measurement_unit_presenter_normalize_verbose_owned(strdup("90\xC2\xA0% dry"));
    char *carats = measurement_unit_presenter_normalize_verbose_owned(
        strdup("carats (one metric carat = 2 x 10<sup>-4</sup>kg) (c/k)"));
    int ok = strcmp(measurement_unit_presenter_resource_type(), "measurement_unit") == 0 &&
             strcmp(measurement_unit_presenter_id(&model), "KGM") == 0 &&
             prefix != NULL &&
             strcmp(prefix, "number of pairs (pa)") == 0 &&
             normalized != NULL &&
             strcmp(normalized, "90% dry") == 0 &&
             carats != NULL &&
             strcmp(carats, "carat (metric) (c/k)") == 0;
    free(prefix);
    free(normalized);
    free(carats);
    return ok;
}
