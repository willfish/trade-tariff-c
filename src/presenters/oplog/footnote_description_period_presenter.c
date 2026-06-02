#include "presenters/oplog/footnote_description_period_presenter.h"

#include <string.h>

const char *footnote_description_period_oplog_presenter_resource_type(void)
{
    return "footnote_description_period";
}

const char *footnote_description_period_oplog_presenter_id(const FootnoteDescriptionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->footnote_description_period_sid;
}

int footnote_description_period_oplog_presenter_self_test(void)
{
    FootnoteDescriptionPeriodOplogModel model = { .footnote_description_period_sid = "id" };
    return strcmp(footnote_description_period_oplog_presenter_resource_type(), "footnote_description_period") == 0 &&
           strcmp(footnote_description_period_oplog_presenter_id(&model), "id") == 0;
}
