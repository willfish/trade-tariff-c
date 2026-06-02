#include "presenters/import_trade_summary_presenter.h"

#include "presenters/measure_duty_expression_presenter.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

const char *import_trade_summary_presenter_resource_type(void)
{
    return "import_trade_summary";
}

const char *import_trade_summary_presenter_id(const ImportTradeSummaryModel *model)
{
    return model == NULL ? NULL : model->id;
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

static int measure_third_country_erga_omnes(const MeasureModel *measure)
{
    return measure_model_import(measure) &&
           (streq(measure->measure_type_id, "103") || streq(measure->measure_type_id, "105")) &&
           streq(measure->geographical_area_id, "1011");
}

static int measure_tariff_preference(const MeasureModel *measure)
{
    return measure_model_import(measure) &&
           (streq(measure->measure_type_id, "142") || streq(measure->measure_type_id, "145"));
}

static int measure_preferential_quota(const MeasureModel *measure)
{
    return measure_model_import(measure) &&
           (streq(measure->measure_type_id, "143") || streq(measure->measure_type_id, "146"));
}

static const MeasureModel *single_matching_measure(const MeasureModelList *measures,
                                                   int (*matches)(const MeasureModel *))
{
    const MeasureModel *match = NULL;
    size_t count = 0;
    if (measures == NULL || matches == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < measures->len; i++) {
        if (matches(&measures->items[i])) {
            match = &measures->items[i];
            count++;
        }
    }
    return count == 1 ? match : NULL;
}

static uint32_t leftrotate(uint32_t value, uint32_t bits)
{
    return (value << bits) | (value >> (32 - bits));
}

static char *md5_hex(const char *input)
{
    static const uint32_t shifts[] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
    };
    static const uint32_t constants[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
    };

    size_t input_len = input == NULL ? 0 : strlen(input);
    uint64_t bit_len = (uint64_t)input_len * 8;
    size_t padded_len = input_len + 1;
    while (padded_len % 64 != 56) {
        padded_len++;
    }
    unsigned char *msg = calloc(padded_len + 8, 1);
    if (msg == NULL) {
        return NULL;
    }
    if (input_len > 0) {
        memcpy(msg, input, input_len);
    }
    msg[input_len] = 0x80;
    for (size_t i = 0; i < 8; i++) {
        msg[padded_len + i] = (unsigned char)((bit_len >> (8 * i)) & 0xff);
    }

    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    for (size_t offset = 0; offset < padded_len + 8; offset += 64) {
        uint32_t words[16];
        for (size_t i = 0; i < 16; i++) {
            words[i] = (uint32_t)msg[offset + i * 4] |
                       ((uint32_t)msg[offset + i * 4 + 1] << 8) |
                       ((uint32_t)msg[offset + i * 4 + 2] << 16) |
                       ((uint32_t)msg[offset + i * 4 + 3] << 24);
        }

        uint32_t a = a0;
        uint32_t b = b0;
        uint32_t c = c0;
        uint32_t d = d0;

        for (uint32_t i = 0; i < 64; i++) {
            uint32_t f;
            uint32_t g;
            if (i < 16) {
                f = (b & c) | (~b & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | (~d & c);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            } else {
                f = c ^ (b | ~d);
                g = (7 * i) % 16;
            }

            uint32_t temp = d;
            d = c;
            c = b;
            b = b + leftrotate(a + f + constants[i] + words[g], shifts[i]);
            a = temp;
        }

        a0 += a;
        b0 += b;
        c0 += c;
        d0 += d;
    }

    free(msg);
    char *hex = malloc(33);
    if (hex == NULL) {
        return NULL;
    }
    snprintf(hex, 33,
             "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
             a0 & 0xff, (a0 >> 8) & 0xff, (a0 >> 16) & 0xff, (a0 >> 24) & 0xff,
             b0 & 0xff, (b0 >> 8) & 0xff, (b0 >> 16) & 0xff, (b0 >> 24) & 0xff,
             c0 & 0xff, (c0 >> 8) & 0xff, (c0 >> 16) & 0xff, (c0 >> 24) & 0xff,
             d0 & 0xff, (d0 >> 8) & 0xff, (d0 >> 16) & 0xff, (d0 >> 24) & 0xff);
    return hex;
}

static char *content_addressable_id(const ImportTradeSummaryModel *model)
{
    size_t len = 3;
    const char *fields[] = {
        model->basic_third_country_duty == NULL ? "" : model->basic_third_country_duty,
        model->preferential_tariff_duty == NULL ? "" : model->preferential_tariff_duty,
        model->preferential_quota_duty == NULL ? "" : model->preferential_quota_duty,
    };
    for (size_t i = 0; i < 3; i++) {
        len += strlen(fields[i]);
    }
    char *content = malloc(len);
    if (content == NULL) {
        return NULL;
    }
    snprintf(content, len, "%s\n%s\n%s", fields[0], fields[1], fields[2]);
    char *id = md5_hex(content);
    free(content);
    return id;
}

int import_trade_summary_presenter_build(const MeasureModelList *measures,
                                         const MeasureComponentModelList *components,
                                         ImportTradeSummaryModel *out)
{
    if (measures == NULL || components == NULL || out == NULL) {
        return 0;
    }

    ImportTradeSummaryModel summary = { 0 };
    const MeasureModel *third_country = single_matching_measure(measures, measure_third_country_erga_omnes);
    const MeasureModel *tariff_preference = single_matching_measure(measures, measure_tariff_preference);
    const MeasureModel *preferential_quota = single_matching_measure(measures, measure_preferential_quota);

    summary.basic_third_country_duty = measure_duty_expression_presenter_formatted(third_country, components);
    summary.preferential_tariff_duty = measure_duty_expression_presenter_formatted(tariff_preference, components);
    summary.preferential_quota_duty = measure_duty_expression_presenter_formatted(preferential_quota, components);
    summary.id = content_addressable_id(&summary);

    if (summary.id == NULL) {
        import_trade_summary_model_free(&summary);
        return 0;
    }

    *out = summary;
    return 1;
}

int import_trade_summary_presenter_self_test(void)
{
    MeasureModelList measures = { 0 };
    MeasureComponentModelList components = { 0 };
    MeasureModel measure = {
        .measure_sid = copy_literal("1"),
        .measure_type_id = copy_literal("103"),
        .geographical_area_id = copy_literal("1011"),
        .trade_movement_code = copy_literal("0"),
    };
    MeasureComponentModel component = {
        .measure_sid = copy_literal("1"),
        .duty_expression_id = copy_literal("01"),
        .duty_amount = copy_literal("8.5"),
    };
    ImportTradeSummaryModel summary = { 0 };
    char *empty_md5 = md5_hex("");
    int ok = empty_md5 != NULL &&
             strcmp(empty_md5, "d41d8cd98f00b204e9800998ecf8427e") == 0 &&
             strcmp(import_trade_summary_presenter_resource_type(), "import_trade_summary") == 0 &&
             import_trade_summary_presenter_id(NULL) == NULL &&
             measure_model_list_append(&measures, measure) &&
             measure_component_model_list_append(&components, component) &&
             import_trade_summary_presenter_build(&measures, &components, &summary) &&
             summary.id != NULL &&
             import_trade_summary_presenter_id(&summary) == summary.id &&
             summary.basic_third_country_duty != NULL &&
             strcmp(summary.basic_third_country_duty, "8.5 %") == 0 &&
             summary.preferential_tariff_duty == NULL &&
             summary.preferential_quota_duty == NULL;

    free(empty_md5);
    import_trade_summary_model_free(&summary);
    measure_model_list_free(&measures);
    measure_component_model_list_free(&components);
    return ok;
}
