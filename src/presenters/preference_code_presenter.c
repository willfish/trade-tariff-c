#include "presenters/preference_code_presenter.h"

#include <stddef.h>
#include <string.h>

typedef struct {
    const char *code;
    const char *description;
} PreferenceCodeDefinition;

static const PreferenceCodeDefinition PREFERENCE_CODES[] = {
    { "100", "Erga Omnes third country duty rates" },
    { "110", "Erga omnes autonomous tariff suspension" },
    { "115", "Erga Omnes autonomous tariff suspensions subject to an end-use" },
    { "119", "Erga Omnes autonomous tariff suspensions subject to an 'airworthiness certificate'" },
    { "120", "Non preferential tariff quotas" },
    { "123", "Non-preferential tariff quotas subject to an end-use authorisation" },
    { "125", "Non preferential tariff quotas subject to a special certificate" },
    { "128", "Erga Omnes tariff quotas following outward processing" },
    { "140", "Special end use resulting from the Common Customs Tariff" },
    { "150", "Admission to CN codes subject to special certificates" },
    { "200", "GSP duty rate without conditions or limits" },
    { "218", "GSP suspensions with certificate confirming the special nature of the product" },
    { "220", "GSP tariff quotas" },
    { "223", "GSP tariff quota subject to an end use" },
    { "225", "GSP tariff quotas subject to a special certificate" },
    { "240", "GSP preferential duty rates subject to an end use" },
    { "250", "Admission to CN codes with special GSP rates subject to a special certificate" },
    { "300", "Preferential duty rate without conditions or limits (including ceilings)" },
    { "310", "Preferential agreements: tariff suspensions" },
    { "315", "Preferential suspension subject to an end use" },
    { "318", "Preferential suspension with certificate confirming the special nature of the product" },
    { "320", "Preferential tariff quotas" },
    { "323", "Preferential tariff quotas subject to an end use" },
    { "325", "Preferential tariff quotas subject to a special certificate" },
    { "340", "Preferential duty rates subject to final destination" },
    { "350", "Admission to CN codes with special preferential rates subject to a special certificate" },
    { "400", "Non-imposition of customs duties under the provisions of customs union agreements concluded by the Community" },
};

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static int string_in_set(const char *value, const char *const *set, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (streq(value, set[i])) {
            return 1;
        }
    }
    return 0;
}

static int gsp_or_dcts_area(const char *geographical_area_id)
{
    static const char *const areas[] = { "2005", "2020", "2027", "1060", "1061", "1062" };
    return string_in_set(geographical_area_id, areas, sizeof(areas) / sizeof(areas[0]));
}

static int condition_belongs_to_measure(const MeasureConditionModel *condition, const MeasureModel *measure)
{
    return condition != NULL &&
           measure != NULL &&
           streq(condition->measure_sid, measure->measure_sid);
}

static int condition_authorised_use(const MeasureConditionModel *condition)
{
    return condition != NULL &&
           streq(condition->certificate_type_code, "N") &&
           streq(condition->certificate_code, "990");
}

static int condition_special_nature(const MeasureConditionModel *condition)
{
    return condition != NULL && streq(condition->certificate_type_code, "A");
}

static int measure_import(const MeasureModel *measure)
{
    return measure != NULL &&
           (streq(measure->trade_movement_code, "0") ||
            streq(measure->trade_movement_code, "2"));
}

const char *preference_code_presenter_description(const char *code)
{
    for (size_t i = 0; i < sizeof(PREFERENCE_CODES) / sizeof(PREFERENCE_CODES[0]); i++) {
        if (streq(PREFERENCE_CODES[i].code, code)) {
            return PREFERENCE_CODES[i].description;
        }
    }
    return NULL;
}

const char *preference_code_presenter_resource_type(void)
{
    return "preference_code";
}

const char *preference_code_presenter_id(const char *code)
{
    return code;
}

const char *preference_code_presenter_determine_code(const PreferenceCodeDeclarable *declarable,
                                                     const PreferenceCodeMeasure *measure)
{
    if (measure == NULL || !measure->import || measure->measure_type_id == NULL) {
        return NULL;
    }

    int declarable_authorised_use = declarable != NULL && declarable->authorised_use_provisions_submission;
    int declarable_special_nature = declarable != NULL && declarable->special_nature;

    if (streq(measure->measure_type_id, "105") || streq(measure->measure_type_id, "117")) {
        return "140";
    }
    if (streq(measure->measure_type_id, "106")) {
        return "400";
    }
    if (streq(measure->measure_type_id, "115")) {
        return "115";
    }
    if (streq(measure->measure_type_id, "117")) {
        return "140";
    }
    if (streq(measure->measure_type_id, "119")) {
        return "119";
    }
    if (streq(measure->measure_type_id, "123")) {
        return "123";
    }
    if (streq(measure->measure_type_id, "103")) {
        if (declarable_authorised_use) {
            return "140";
        }
        return declarable_special_nature ? "150" : "100";
    }
    if (streq(measure->measure_type_id, "112")) {
        return measure->authorised_use ? "115" : "110";
    }
    if (streq(measure->measure_type_id, "122")) {
        if (declarable_special_nature) {
            return "125";
        }
        return measure->authorised_use ? "123" : "120";
    }
    if (streq(measure->measure_type_id, "141")) {
        return measure->authorised_use ? "315" : "310";
    }
    if (streq(measure->measure_type_id, "142")) {
        if (measure->gsp_or_dcts) {
            return measure->authorised_use ? "240" : "200";
        }
        return measure->authorised_use ? "340" : "300";
    }
    if (streq(measure->measure_type_id, "143")) {
        if (measure->gsp_or_dcts) {
            if (declarable_special_nature) {
                return "255";
            }
            return measure->authorised_use ? "223" : "220";
        }
        if (declarable_special_nature) {
            return "325";
        }
        return measure->authorised_use ? "323" : "320";
    }
    if (streq(measure->measure_type_id, "145")) {
        return measure->gsp_or_dcts ? "240" : "340";
    }
    if (streq(measure->measure_type_id, "146")) {
        return measure->gsp_or_dcts ? "223" : "323";
    }

    return NULL;
}

PreferenceCodeDeclarable preference_code_presenter_declarable_from_measures(const MeasureModelList *measures,
                                                                            const MeasureConditionModelList *conditions)
{
    PreferenceCodeDeclarable declarable = { 0 };
    if (measures == NULL) {
        return declarable;
    }

    for (size_t i = 0; i < measures->len; i++) {
        const MeasureModel *measure = &measures->items[i];
        if (!measure_import(measure)) {
            continue;
        }

        if (streq(measure->measure_type_id, "464")) {
            declarable.authorised_use_provisions_submission = 1;
        }

        if (conditions == NULL) {
            continue;
        }
        for (size_t j = 0; j < conditions->len; j++) {
            const MeasureConditionModel *condition = &conditions->items[j];
            if (condition_belongs_to_measure(condition, measure) && condition_special_nature(condition)) {
                declarable.special_nature = 1;
            }
        }
    }

    return declarable;
}

const char *preference_code_presenter_code_for_measure(const PreferenceCodeDeclarable *declarable,
                                                       const MeasureModel *measure,
                                                       const MeasureConditionModelList *conditions)
{
    if (measure == NULL) {
        return NULL;
    }

    PreferenceCodeMeasure preference_measure = {
        .measure_type_id = measure->measure_type_id,
        .geographical_area_id = measure->geographical_area_id,
        .import = measure_import(measure),
        .authorised_use = 0,
        .gsp_or_dcts = gsp_or_dcts_area(measure->geographical_area_id),
    };

    if (conditions != NULL) {
        for (size_t i = 0; i < conditions->len; i++) {
            const MeasureConditionModel *condition = &conditions->items[i];
            if (condition_belongs_to_measure(condition, measure) && condition_authorised_use(condition)) {
                preference_measure.authorised_use = 1;
                break;
            }
        }
    }

    const char *code = preference_code_presenter_determine_code(declarable, &preference_measure);
    return preference_code_presenter_description(code) == NULL ? NULL : code;
}

int preference_code_presenter_self_test(void)
{
    PreferenceCodeDeclarable declarable = { 0 };
    PreferenceCodeMeasure suspension = {
        .measure_type_id = "112",
        .geographical_area_id = "1011",
        .import = 1,
        .authorised_use = 0,
        .gsp_or_dcts = 0,
    };
    PreferenceCodeMeasure authorised_suspension = suspension;
    authorised_suspension.authorised_use = 1;

    return streq(preference_code_presenter_determine_code(&declarable, &suspension), "110") &&
           streq(preference_code_presenter_determine_code(&declarable, &authorised_suspension), "115") &&
           streq(preference_code_presenter_description("110"), "Erga omnes autonomous tariff suspension");
}
