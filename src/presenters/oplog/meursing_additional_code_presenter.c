#include "presenters/oplog/meursing_additional_code_presenter.h"

#include <string.h>

const char *meursing_additional_code_oplog_presenter_resource_type(void)
{
    return "meursing_additional_code";
}

const char *meursing_additional_code_oplog_presenter_id(const MeursingAdditionalCodeOplogModel *model)
{
    return model == NULL ? NULL : model->meursing_additional_code_sid;
}

int meursing_additional_code_oplog_presenter_self_test(void)
{
    MeursingAdditionalCodeOplogModel model = { .meursing_additional_code_sid = "id" };
    return strcmp(meursing_additional_code_oplog_presenter_resource_type(), "meursing_additional_code") == 0 &&
           strcmp(meursing_additional_code_oplog_presenter_id(&model), "id") == 0;
}
