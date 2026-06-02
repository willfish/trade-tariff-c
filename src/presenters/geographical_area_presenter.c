#include "presenters/geographical_area_presenter.h"

#include <string.h>

const char *geographical_area_presenter_resource_type(void)
{
    return "geographical_area";
}

const char *geographical_area_presenter_id(const GeographicalAreaModel *model)
{
    return model == NULL ? NULL : model->geographical_area_id;
}

const char *geographical_area_presenter_excluded_id(const MeasureExcludedGeographicalAreaModel *model)
{
    return model == NULL ? NULL : model->geographical_area_id;
}

int geographical_area_presenter_self_test(void)
{
    GeographicalAreaModel model = { .geographical_area_id = "1011" };
    MeasureExcludedGeographicalAreaModel excluded = { .geographical_area_id = "AD" };
    return strcmp(geographical_area_presenter_resource_type(), "geographical_area") == 0 &&
           strcmp(geographical_area_presenter_id(&model), "1011") == 0 &&
           geographical_area_presenter_id(NULL) == NULL &&
           strcmp(geographical_area_presenter_excluded_id(&excluded), "AD") == 0 &&
           geographical_area_presenter_excluded_id(NULL) == NULL;
}
