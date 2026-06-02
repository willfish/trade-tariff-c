#include "presenters/oplog/meursing_heading_text_presenter.h"

#include <string.h>

const char *meursing_heading_text_oplog_presenter_resource_type(void)
{
    return "meursing_heading_text";
}

const char *meursing_heading_text_oplog_presenter_id(const MeursingHeadingTextOplogModel *model)
{
    return model == NULL ? NULL : model->meursing_table_plan_id;
}

int meursing_heading_text_oplog_presenter_self_test(void)
{
    MeursingHeadingTextOplogModel model = { .meursing_table_plan_id = "id" };
    return strcmp(meursing_heading_text_oplog_presenter_resource_type(), "meursing_heading_text") == 0 &&
           strcmp(meursing_heading_text_oplog_presenter_id(&model), "id") == 0;
}
