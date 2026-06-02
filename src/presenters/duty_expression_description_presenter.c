#include "presenters/duty_expression_description_presenter.h"

#include <string.h>

typedef struct {
    const char *id;
    const char *abbreviation;
} DutyExpressionAbbreviation;

static const DutyExpressionAbbreviation ABBREVIATIONS[] = {
    { "01", "%" },
    { "02", "-" },
    { "04", "+" },
    { "12", "+ EA" },
    { "14", "+ EAR" },
    { "15", "MIN" },
    { "17", "MAX" },
    { "19", "+" },
    { "20", "+" },
    { "21", "+ADSZ" },
    { "25", "+ADSZR" },
    { "27", "+ADFM" },
    { "29", "+ADFMR" },
    { "35", "MAX" },
    { "36", "-" },
    { "37", "NIHIL" },
    { "40", "ERCER" },
    { "41", "ERRIS" },
    { "42", "EREGG" },
    { "43", "ERSUG" },
    { "44", "ERMLK" },
    { "99", "UNSUP" },
};

const char *duty_expression_description_presenter_abbreviation(const DutyExpressionDescriptionModel *model)
{
    if (model == NULL || model->duty_expression_id == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < sizeof(ABBREVIATIONS) / sizeof(ABBREVIATIONS[0]); i++) {
        if (strcmp(model->duty_expression_id, ABBREVIATIONS[i].id) == 0) {
            return ABBREVIATIONS[i].abbreviation;
        }
    }
    return NULL;
}

int duty_expression_description_presenter_self_test(void)
{
    DutyExpressionDescriptionModel percent = { .duty_expression_id = "01" };
    DutyExpressionDescriptionModel unsupported = { .duty_expression_id = "99" };
    DutyExpressionDescriptionModel unknown = { .duty_expression_id = "XX" };
    return strcmp(duty_expression_description_presenter_abbreviation(&percent), "%") == 0 &&
           strcmp(duty_expression_description_presenter_abbreviation(&unsupported), "UNSUP") == 0 &&
           duty_expression_description_presenter_abbreviation(&unknown) == NULL;
}
