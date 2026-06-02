#include "presenters/oplog/measure_type_series_description_presenter.h"

#include <string.h>

const char *measure_type_series_description_oplog_presenter_resource_type(void)
{
    return "measure_type_series_description";
}

const char *measure_type_series_description_oplog_presenter_id(const MeasureTypeSeriesDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->measure_type_series_id;
}

int measure_type_series_description_oplog_presenter_self_test(void)
{
    MeasureTypeSeriesDescriptionOplogModel model = { .measure_type_series_id = "id" };
    return strcmp(measure_type_series_description_oplog_presenter_resource_type(), "measure_type_series_description") == 0 &&
           strcmp(measure_type_series_description_oplog_presenter_id(&model), "id") == 0;
}
