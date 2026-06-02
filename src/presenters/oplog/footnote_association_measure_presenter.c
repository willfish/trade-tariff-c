#include "presenters/oplog/footnote_association_measure_presenter.h"

#include <string.h>

const char *footnote_association_measure_oplog_presenter_resource_type(void)
{
    return "footnote_association_measure";
}

const char *footnote_association_measure_oplog_presenter_id(const FootnoteAssociationMeasureOplogModel *model)
{
    return model == NULL ? NULL : model->measure_sid;
}

int footnote_association_measure_oplog_presenter_self_test(void)
{
    FootnoteAssociationMeasureOplogModel model = { .measure_sid = "id" };
    return strcmp(footnote_association_measure_oplog_presenter_resource_type(), "footnote_association_measure") == 0 &&
           strcmp(footnote_association_measure_oplog_presenter_id(&model), "id") == 0;
}
