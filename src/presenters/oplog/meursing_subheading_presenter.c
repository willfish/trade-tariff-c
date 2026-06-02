#include "presenters/oplog/meursing_subheading_presenter.h"

#include <string.h>

const char *meursing_subheading_oplog_presenter_resource_type(void)
{
    return "meursing_subheading";
}

const char *meursing_subheading_oplog_presenter_id(const MeursingSubheadingOplogModel *model)
{
    return model == NULL ? NULL : model->meursing_table_plan_id;
}

int meursing_subheading_oplog_presenter_self_test(void)
{
    MeursingSubheadingOplogModel model = { .meursing_table_plan_id = "id" };
    return strcmp(meursing_subheading_oplog_presenter_resource_type(), "meursing_subheading") == 0 &&
           strcmp(meursing_subheading_oplog_presenter_id(&model), "id") == 0;
}
