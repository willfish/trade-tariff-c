#include "presenters/measure_type_presenter.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

const char *measure_type_presenter_resource_type(void)
{
    return "measure_type";
}

const char *measure_type_presenter_id(const MeasureTypeModel *model)
{
    return model == NULL ? NULL : model->measure_type_id;
}

MeasureTypePresentedCode measure_type_presenter_code(const char *value)
{
    if (value == NULL || *value == '\0') {
        return (MeasureTypePresentedCode){ .is_null = 1 };
    }

    char *end = NULL;
    errno = 0;
    long parsed = strtol(value, &end, 10);
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return (MeasureTypePresentedCode){ .string_value = value };
    }

    return (MeasureTypePresentedCode){ .is_int = 1, .int_value = parsed };
}

int measure_type_presenter_self_test(void)
{
    MeasureTypeModel model = { .measure_type_id = "103" };
    MeasureTypePresentedCode numeric = measure_type_presenter_code("2");
    MeasureTypePresentedCode blank = measure_type_presenter_code("");
    MeasureTypePresentedCode fallback = measure_type_presenter_code("A");

    return strcmp(measure_type_presenter_resource_type(), "measure_type") == 0 &&
           strcmp(measure_type_presenter_id(&model), "103") == 0 &&
           measure_type_presenter_id(NULL) == NULL &&
           numeric.is_int &&
           numeric.int_value == 2 &&
           blank.is_null &&
           !fallback.is_null &&
           !fallback.is_int &&
           strcmp(fallback.string_value, "A") == 0;
}
