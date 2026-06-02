#include "presenters/oplog/measure_type_series_presenter.h"

#include <string.h>

const char *measure_type_series_oplog_presenter_resource_type(void)
{
    return "measure_type_series";
}

const char *measure_type_series_oplog_presenter_id(const MeasureTypeSeriesOplogModel *model)
{
    return model == NULL ? NULL : model->measure_type_series_id;
}

int measure_type_series_oplog_presenter_self_test(void)
{
    MeasureTypeSeriesOplogModel model = { .measure_type_series_id = "id" };
    return strcmp(measure_type_series_oplog_presenter_resource_type(), "measure_type_series") == 0 &&
           strcmp(measure_type_series_oplog_presenter_id(&model), "id") == 0;
}
