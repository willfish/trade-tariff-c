#include "presenters/oplog/meursing_table_plan_presenter.h"

#include <string.h>

const char *meursing_table_plan_oplog_presenter_resource_type(void)
{
    return "meursing_table_plan";
}

const char *meursing_table_plan_oplog_presenter_id(const MeursingTablePlanOplogModel *model)
{
    return model == NULL ? NULL : model->meursing_table_plan_id;
}

int meursing_table_plan_oplog_presenter_self_test(void)
{
    MeursingTablePlanOplogModel model = { .meursing_table_plan_id = "id" };
    return strcmp(meursing_table_plan_oplog_presenter_resource_type(), "meursing_table_plan") == 0 &&
           strcmp(meursing_table_plan_oplog_presenter_id(&model), "id") == 0;
}
