#include "presenters/oplog/meursing_heading_presenter.h"

#include <string.h>

const char *meursing_heading_oplog_presenter_resource_type(void)
{
    return "meursing_heading";
}

const char *meursing_heading_oplog_presenter_id(const MeursingHeadingOplogModel *model)
{
    return model == NULL ? NULL : model->meursing_table_plan_id;
}

int meursing_heading_oplog_presenter_self_test(void)
{
    MeursingHeadingOplogModel model = { .meursing_table_plan_id = "id" };
    return strcmp(meursing_heading_oplog_presenter_resource_type(), "meursing_heading") == 0 &&
           strcmp(meursing_heading_oplog_presenter_id(&model), "id") == 0;
}
