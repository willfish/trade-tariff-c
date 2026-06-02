#include "presenters/additional_code_presenter.h"

#include "formatters/description_formatter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *additional_code_presenter_resource_type(void)
{
    return "additional_code";
}

const char *additional_code_presenter_id(const AdditionalCodeModel *model)
{
    return model == NULL ? NULL : model->additional_code_sid;
}

char *additional_code_presenter_code(const AdditionalCodeModel *model)
{
    if (model == NULL || model->additional_code_type_id == NULL || model->additional_code == NULL) {
        return NULL;
    }

    size_t len = strlen(model->additional_code_type_id) + strlen(model->additional_code) + 1;
    char *code = malloc(len);
    if (code == NULL) {
        return NULL;
    }
    snprintf(code, len, "%s%s", model->additional_code_type_id, model->additional_code);
    return code;
}

char *additional_code_presenter_description(const AdditionalCodeModel *model, int uk)
{
    return description_formatter_format(model == NULL ? NULL : model->description, uk);
}

int additional_code_presenter_self_test(void)
{
    AdditionalCodeModel model = { .additional_code_sid = "321" };
    AdditionalCodeModel described = {
        .additional_code_type_id = "8",
        .additional_code = "123",
        .description = "All imports of ceramic tiles that, individually, have: <br><br><br>a largest surface area exceeding 0.36m2"
    };
    char *code = additional_code_presenter_code(&described);
    char *description = additional_code_presenter_description(&described, 1);
    int ok = code != NULL &&
             description != NULL &&
             strcmp(additional_code_presenter_resource_type(), "additional_code") == 0 &&
             strcmp(additional_code_presenter_id(&model), "321") == 0 &&
             strcmp(code, "8123") == 0 &&
             strcmp(description, "All imports of ceramic tiles that, individually, have: <br><br>a largest surface area exceeding 0.36m2") == 0;
    free(code);
    free(description);
    return ok;
}
