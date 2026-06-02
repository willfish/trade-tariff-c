#include "presenters/oplog/measure_excluded_geographical_area_presenter.h"

#include <string.h>

const char *measure_excluded_geographical_area_oplog_presenter_resource_type(void)
{
    return "measure_excluded_geographical_area";
}

const char *measure_excluded_geographical_area_oplog_presenter_id(const MeasureExcludedGeographicalAreaOplogModel *model)
{
    return model == NULL ? NULL : model->measure_sid;
}

int measure_excluded_geographical_area_oplog_presenter_self_test(void)
{
    MeasureExcludedGeographicalAreaOplogModel model = { .measure_sid = "id" };
    return strcmp(measure_excluded_geographical_area_oplog_presenter_resource_type(), "measure_excluded_geographical_area") == 0 &&
           strcmp(measure_excluded_geographical_area_oplog_presenter_id(&model), "id") == 0;
}
