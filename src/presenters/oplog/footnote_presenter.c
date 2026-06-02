#include "presenters/oplog/footnote_presenter.h"

#include <string.h>

const char *footnote_oplog_presenter_resource_type(void)
{
    return "footnote";
}

const char *footnote_oplog_presenter_id(const FootnoteOplogModel *model)
{
    return model == NULL ? NULL : model->footnote_id;
}

int footnote_oplog_presenter_self_test(void)
{
    FootnoteOplogModel model = { .footnote_id = "id" };
    return strcmp(footnote_oplog_presenter_resource_type(), "footnote") == 0 &&
           strcmp(footnote_oplog_presenter_id(&model), "id") == 0;
}
