#include "presenters/measure_legal_act_presenter.h"

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

const char *measure_legal_act_presenter_resource_type(void)
{
    return "legal_act";
}

const char *measure_legal_act_presenter_suspension_resource_type(void)
{
    return "suspension_legal_act";
}

const char *measure_legal_act_presenter_id(const MeasureLegalActModel *model)
{
    return model == NULL ? NULL : model->regulation_id;
}

int measure_legal_act_presenter_is_suspension(const MeasureLegalActModel *model)
{
    return model != NULL && streq(model->source, "suspension");
}

int measure_legal_act_presenter_reduced_info(const MeasureLegalActModel *model)
{
    return model != NULL &&
           (streq(model->regulation_id, "IYY99990") ||
            streq(model->measure_type_id, "305") ||
            streq(model->measure_type_id, "306"));
}

static char *regulation_code_for_id(const char *regulation_id)
{
    if (regulation_id == NULL || strlen(regulation_id) < 7) {
        return copy_literal("");
    }

    char *code = malloc(9);
    if (code == NULL) {
        return NULL;
    }
    snprintf(code, 9, "%c%.4s/%.2s", regulation_id[0], regulation_id + 3, regulation_id + 1);
    return code;
}

static char *regulation_url_for_id(const char *regulation_id)
{
    if (regulation_id == NULL || strlen(regulation_id) < 7) {
        return copy_literal("");
    }

    const char *year = regulation_id + 1;
    const char *century = (year[0] > '7' || (year[0] == '7' && year[1] > '0')) ? "19" : "20";
    size_t len = strlen("https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX%3A3") +
                 4 + 1 + 4 + 1;
    char *url = malloc(len);
    if (url == NULL) {
        return NULL;
    }
    snprintf(url,
             len,
             "https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX%%3A3%s%.2s%c%.4s",
             century,
             year,
             regulation_id[0],
             regulation_id + 3);
    return url;
}

char *measure_legal_act_presenter_regulation_code(const MeasureLegalActModel *model)
{
    return measure_legal_act_presenter_reduced_info(model)
               ? copy_literal("")
               : regulation_code_for_id(measure_legal_act_presenter_id(model));
}

char *measure_legal_act_presenter_regulation_url(const MeasureLegalActModel *model)
{
    return measure_legal_act_presenter_reduced_info(model)
               ? copy_literal("")
               : regulation_url_for_id(measure_legal_act_presenter_id(model));
}

char *measure_legal_act_presenter_suspension_regulation_code(const MeasureLegalActModel *model)
{
    return regulation_code_for_id(measure_legal_act_presenter_id(model));
}

char *measure_legal_act_presenter_suspension_regulation_url(const MeasureLegalActModel *model)
{
    return regulation_url_for_id(measure_legal_act_presenter_id(model));
}

const char *measure_legal_act_presenter_description(const MeasureLegalActModel *model)
{
    if (model == NULL || measure_legal_act_presenter_reduced_info(model)) {
        return NULL;
    }
    return model->information_text;
}

const MeasureLegalActModel *measure_legal_act_presenter_suspension_for_measure(
    const MeasureLegalActModelList *legal_acts,
    const char *measure_sid)
{
    if (legal_acts == NULL || measure_sid == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < legal_acts->len; i++) {
        const MeasureLegalActModel *legal_act = &legal_acts->items[i];
        if (streq(legal_act->measure_sid, measure_sid) &&
            measure_legal_act_presenter_is_suspension(legal_act)) {
            return legal_act;
        }
    }
    return NULL;
}

int measure_legal_act_presenter_self_test(void)
{
    MeasureLegalActModel model = {
        .measure_sid = "123",
        .measure_type_id = "103",
        .regulation_id = "R230001",
        .source = "generating",
        .information_text = "Regulation text",
    };
    char *code = measure_legal_act_presenter_regulation_code(&model);
    char *url = measure_legal_act_presenter_regulation_url(&model);
    int ok = strcmp(measure_legal_act_presenter_resource_type(), "legal_act") == 0 &&
             strcmp(measure_legal_act_presenter_suspension_resource_type(), "suspension_legal_act") == 0 &&
             strcmp(measure_legal_act_presenter_id(&model), "R230001") == 0 &&
             strcmp(code == NULL ? "" : code, "R0001/23") == 0 &&
             strstr(url == NULL ? "" : url, "CELEX%3A32023R0001") != NULL &&
             strcmp(measure_legal_act_presenter_description(&model), "Regulation text") == 0;
    free(code);
    free(url);
    return ok;
}
