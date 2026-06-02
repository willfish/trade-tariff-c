#include "presenters/oplog/footnote_description_presenter.h"

#include <string.h>

const char *footnote_description_oplog_presenter_resource_type(void)
{
    return "footnote_description";
}

const char *footnote_description_oplog_presenter_id(const FootnoteDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->footnote_id;
}

int footnote_description_oplog_presenter_self_test(void)
{
    FootnoteDescriptionOplogModel model = { .footnote_id = "id" };
    return strcmp(footnote_description_oplog_presenter_resource_type(), "footnote_description") == 0 &&
           strcmp(footnote_description_oplog_presenter_id(&model), "id") == 0;
}
