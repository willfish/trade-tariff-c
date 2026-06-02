#include "presenters/oplog/footnote_type_presenter.h"

#include <string.h>

const char *footnote_type_oplog_presenter_resource_type(void)
{
    return "footnote_type";
}

const char *footnote_type_oplog_presenter_id(const FootnoteTypeOplogModel *model)
{
    return model == NULL ? NULL : model->footnote_type_id;
}

int footnote_type_oplog_presenter_self_test(void)
{
    FootnoteTypeOplogModel model = { .footnote_type_id = "id" };
    return strcmp(footnote_type_oplog_presenter_resource_type(), "footnote_type") == 0 &&
           strcmp(footnote_type_oplog_presenter_id(&model), "id") == 0;
}
