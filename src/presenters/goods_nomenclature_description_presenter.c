#include "presenters/goods_nomenclature_description_presenter.h"

#include "formatters/description_formatter.h"

#include <ctype.h>
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

static char *replace_prefix_owned(char *input, const char *replacement)
{
    size_t input_len = strlen(input);
    size_t replacement_len = strlen(replacement);
    char *output = malloc(input_len - 2 + replacement_len + 1);
    if (output == NULL) {
        free(input);
        return NULL;
    }
    memcpy(output, replacement, replacement_len);
    strcpy(output + replacement_len, input + 2);
    free(input);
    return output;
}

static char *utf8_downcase_owned(char *input)
{
    for (char *p = input; *p != '\0'; p++) {
        *p = (char)tolower((unsigned char)*p);
    }

    static const struct {
        const char *upper;
        const char *lower;
    } greek_case_pairs[] = {
        { "\xCE\x91", "\xCE\xB1" }, { "\xCE\x92", "\xCE\xB2" },
        { "\xCE\x93", "\xCE\xB3" }, { "\xCE\x94", "\xCE\xB4" },
        { "\xCE\x95", "\xCE\xB5" }, { "\xCE\x96", "\xCE\xB6" },
        { "\xCE\x97", "\xCE\xB7" }, { "\xCE\x98", "\xCE\xB8" },
        { "\xCE\x99", "\xCE\xB9" }, { "\xCE\x9A", "\xCE\xBA" },
        { "\xCE\x9B", "\xCE\xBB" }, { "\xCE\x9C", "\xCE\xBC" },
        { "\xCE\x9D", "\xCE\xBD" }, { "\xCE\x9E", "\xCE\xBE" },
        { "\xCE\x9F", "\xCE\xBF" }, { "\xCE\xA0", "\xCF\x80" },
        { "\xCE\xA1", "\xCF\x81" }, { "\xCE\xA3", "\xCF\x83" },
        { "\xCE\xA4", "\xCF\x84" }, { "\xCE\xA5", "\xCF\x85" },
        { "\xCE\xA6", "\xCF\x86" }, { "\xCE\xA7", "\xCF\x87" },
        { "\xCE\xA8", "\xCF\x88" }, { "\xCE\xA9", "\xCF\x89" },
    };

    for (size_t i = 0; i < sizeof(greek_case_pairs) / sizeof(greek_case_pairs[0]); i++) {
        input = replace_all_owned(input, greek_case_pairs[i].upper, greek_case_pairs[i].lower);
        if (input == NULL) {
            return NULL;
        }
    }
    return input;
}

static char *goods_description_capitalize_owned(char *input)
{
    if (input == NULL || input[0] == '\0') {
        return input;
    }
    if ((unsigned char)input[0] == 0xC3 && (unsigned char)input[1] == 0x9F) {
        return replace_all_owned(input, "\xC3\x9F", "Ss");
    }
    static const struct {
        const char *lower;
        const char *upper;
    } greek_case_pairs[] = {
        { "\xCE\xB1", "\xCE\x91" }, { "\xCE\xB2", "\xCE\x92" },
        { "\xCE\xB3", "\xCE\x93" }, { "\xCE\xB4", "\xCE\x94" },
        { "\xCE\xB5", "\xCE\x95" }, { "\xCE\xB6", "\xCE\x96" },
        { "\xCE\xB7", "\xCE\x97" }, { "\xCE\xB8", "\xCE\x98" },
        { "\xCE\xB9", "\xCE\x99" }, { "\xCE\xBA", "\xCE\x9A" },
        { "\xCE\xBB", "\xCE\x9B" }, { "\xCE\xBC", "\xCE\x9C" },
        { "\xCE\xBD", "\xCE\x9D" }, { "\xCE\xBE", "\xCE\x9E" },
        { "\xCE\xBF", "\xCE\x9F" }, { "\xCF\x80", "\xCE\xA0" },
        { "\xCF\x81", "\xCE\xA1" }, { "\xCF\x83", "\xCE\xA3" },
        { "\xCF\x84", "\xCE\xA4" }, { "\xCF\x85", "\xCE\xA5" },
        { "\xCF\x86", "\xCE\xA6" }, { "\xCF\x87", "\xCE\xA7" },
        { "\xCF\x88", "\xCE\xA8" }, { "\xCF\x89", "\xCE\xA9" },
    };
    for (size_t i = 0; i < sizeof(greek_case_pairs) / sizeof(greek_case_pairs[0]); i++) {
        if (strncmp(input, greek_case_pairs[i].lower, 2) == 0) {
            return replace_prefix_owned(input, greek_case_pairs[i].upper);
        }
    }
    input[0] = (char)toupper((unsigned char)input[0]);
    return input;
}

static int is_country_char(unsigned char c)
{
    return isalpha(c) || c == ',' || c == '\'' || isspace(c) || c >= 0x80;
}

static const char *match_consigned_marker_at(const char *cursor, size_t *marker_len)
{
    static const char *markers[] = {
        "consigned from or originating in",
        "consigned from",
    };

    for (size_t i = 0; i < sizeof(markers) / sizeof(markers[0]); i++) {
        size_t len = strlen(markers[i]);
        size_t matched = 0;
        while (matched < len && cursor[matched] != '\0') {
            unsigned char a = (unsigned char)cursor[matched];
            unsigned char b = (unsigned char)markers[i][matched];
            if (a < 0x80 && b < 0x80) {
                if (tolower(a) != tolower(b)) {
                    break;
                }
            } else if (a != b) {
                break;
            }
            matched++;
        }
        if (matched == len) {
            *marker_len = len;
            return cursor;
        }
    }
    return NULL;
}

static char *extract_consigned_country_at(const char *match, size_t marker_len)
{
    const char *start = match + marker_len;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }
    const char *end = start;
    while (*end != '\0' && is_country_char((unsigned char)*end)) {
        end++;
    }
    while (end > start && isspace((unsigned char)end[-1])) {
        end--;
    }
    if (end == start) {
        return NULL;
    }

    size_t len = (size_t)(end - start);
    char *country = malloc(len + 1);
    if (country == NULL) {
        return NULL;
    }
    memcpy(country, start, len);
    country[len] = '\0';
    return country;
}

static int append_consigned_country(char **buffer, size_t *len, size_t *cap, const char *country)
{
    if (buffer == NULL || len == NULL || cap == NULL || country == NULL || *country == '\0') {
        return 1;
    }

    size_t country_len = strlen(country);
    size_t separator_len = *len == 0 ? 0 : 2;
    if (*len + separator_len + country_len + 1 > *cap) {
        size_t next_cap = *cap == 0 ? 64 : *cap;
        while (*len + separator_len + country_len + 1 > next_cap) {
            next_cap *= 2;
        }
        char *next = realloc(*buffer, next_cap);
        if (next == NULL) {
            return 0;
        }
        *buffer = next;
        *cap = next_cap;
    }

    if (separator_len > 0) {
        memcpy(*buffer + *len, ", ", separator_len);
        *len += separator_len;
    }
    memcpy(*buffer + *len, country, country_len);
    *len += country_len;
    (*buffer)[*len] = '\0';
    return 1;
}

char *goods_nomenclature_description_presenter_consigned_from(const GoodsNomenclatureDescriptionModel *model)
{
    const char *description = model == NULL ? NULL : model->description;
    if (description == NULL || *description == '\0') {
        return NULL;
    }

    char *buffer = NULL;
    size_t len = 0;
    size_t cap = 0;
    for (const char *cursor = description; *cursor != '\0'; cursor++) {
        size_t marker_len = 0;
        const char *match = match_consigned_marker_at(cursor, &marker_len);
        if (match == NULL) {
            continue;
        }

        char *country = extract_consigned_country_at(match, marker_len);
        int ok = append_consigned_country(&buffer, &len, &cap, country);
        free(country);
        if (!ok) {
            free(buffer);
            return NULL;
        }
        cursor = match + marker_len - 1;
    }

    return buffer;
}

static char *restore_all_consigned_countries_owned(char *formatted, const char *description)
{
    const char *cursor = description;
    while (*cursor != '\0') {
        size_t marker_len = 0;
        const char *match = match_consigned_marker_at(cursor, &marker_len);
        if (match == NULL) {
            cursor++;
            continue;
        }

        char *country = extract_consigned_country_at(match, marker_len);
        if (country == NULL) {
            cursor = match + marker_len;
            continue;
        }

        char *lower_country = malloc(strlen(country) + 1);
        if (lower_country == NULL) {
            free(country);
            free(formatted);
            return NULL;
        }
        strcpy(lower_country, country);
        lower_country = utf8_downcase_owned(lower_country);
        if (lower_country == NULL) {
            free(country);
            free(formatted);
            return NULL;
        }

        formatted = replace_all_owned(formatted, lower_country, country);
        free(lower_country);
        free(country);
        if (formatted == NULL) {
            return NULL;
        }
        cursor = match + marker_len;
    }
    return formatted;
}

static char *restore_consigned_country_owned(char *formatted, const char *description)
{
    char *restored = restore_all_consigned_countries_owned(formatted, description);
    if (restored != NULL) {
        return restored;
    }
    return NULL;
}

char *goods_nomenclature_description_presenter_description(const GoodsNomenclatureDescriptionModel *model)
{
    return description_formatter_normalize_goods_description(model == NULL ? NULL : model->description);
}

char *goods_nomenclature_description_presenter_description_plain(const GoodsNomenclatureDescriptionModel *model)
{
    char *description = goods_nomenclature_description_presenter_description(model);
    if (description == NULL) {
        return NULL;
    }
    char *trimmed = description_formatter_trim_plain(description);
    free(description);
    if (trimmed == NULL) {
        return NULL;
    }
    description = trimmed;
    description = utf8_downcase_owned(description);
    if (description == NULL) {
        return NULL;
    }
    return goods_description_capitalize_owned(description);
}

char *goods_nomenclature_description_presenter_formatted_description(const GoodsNomenclatureDescriptionModel *model)
{
    int uk = goods_nomenclature_description_uk(model);
    char *description = goods_nomenclature_description_presenter_description(model);
    if (description == NULL) {
        return NULL;
    }
    char *formatted = description_formatter_format(description, uk);
    if (formatted == NULL) {
        free(description);
        return NULL;
    }
    formatted = utf8_downcase_owned(formatted);
    if (formatted == NULL) {
        free(description);
        return NULL;
    }
    formatted = goods_description_capitalize_owned(formatted);
    if (formatted == NULL) {
        free(description);
        return NULL;
    }
    formatted = restore_consigned_country_owned(formatted, description);
    free(description);
    return formatted;
}

int goods_nomenclature_description_presenter_self_test(void)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(
        "In blocks with 7.0 \xCE\xBC.\xCE\xA9.m<br>                            <br><br>-\xC2\xA0tail",
        "uk");
    char *description = goods_nomenclature_description_presenter_description(&model);
    char *plain = goods_nomenclature_description_presenter_description_plain(&model);
    char *formatted = goods_nomenclature_description_presenter_formatted_description(&model);
    int ok = description != NULL && plain != NULL && formatted != NULL &&
             strstr(description, "<br>                           <br>-\xC2\xA0tail") != NULL &&
             strstr(plain, "\xCE\xBC.\xCF\x89.m") != NULL &&
             strstr(formatted, "\xCE\xBC.\xCF\x89.m") != NULL &&
             strstr(plain, "$") == NULL;
    free(description);
    free(plain);
    free(formatted);

    GoodsNomenclatureDescriptionModel consigned = goods_nomenclature_description_new("Consigned from T\xC3\xBCrkiye", "uk");
    formatted = goods_nomenclature_description_presenter_formatted_description(&consigned);
    ok = ok && formatted != NULL && strcmp(formatted, "Consigned from T\xC3\xBCrkiye") == 0;
    free(formatted);
    char *consigned_from = goods_nomenclature_description_presenter_consigned_from(&consigned);
    ok = ok && consigned_from != NULL && strcmp(consigned_from, "T\xC3\xBCrkiye") == 0;
    free(consigned_from);

    GoodsNomenclatureDescriptionModel glass = goods_nomenclature_description_new(
        "Of other glass having a linear coefficient of expansion not exceeding 5\xC2\xA0\xC3\x97\xC2\xA0" "10$-<sup>6</sup>",
        "uk");
    plain = goods_nomenclature_description_presenter_description_plain(&glass);
    ok = ok && plain != NULL && strstr(plain, "10-<sup>6</sup>") != NULL && strstr(plain, "$") == NULL;
    free(plain);
    return ok;
}
