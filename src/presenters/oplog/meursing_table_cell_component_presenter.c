#include "presenters/oplog/meursing_table_cell_component_presenter.h"

#include <string.h>

const char *meursing_table_cell_component_oplog_presenter_resource_type(void)
{
    return "meursing_table_cell_component";
}

const char *meursing_table_cell_component_oplog_presenter_id(const MeursingTableCellComponentOplogModel *model)
{
    return model == NULL ? NULL : model->meursing_additional_code_sid;
}

int meursing_table_cell_component_oplog_presenter_self_test(void)
{
    MeursingTableCellComponentOplogModel model = { .meursing_additional_code_sid = "id" };
    return strcmp(meursing_table_cell_component_oplog_presenter_resource_type(), "meursing_table_cell_component") == 0 &&
           strcmp(meursing_table_cell_component_oplog_presenter_id(&model), "id") == 0;
}
