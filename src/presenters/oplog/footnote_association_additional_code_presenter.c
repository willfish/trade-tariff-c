#include "presenters/oplog/footnote_association_additional_code_presenter.h"

#include <string.h>

const char *footnote_association_additional_code_oplog_presenter_resource_type(void)
{
    return "footnote_association_additional_code";
}

const char *footnote_association_additional_code_oplog_presenter_id(const FootnoteAssociationAdditionalCodeOplogModel *model)
{
    return model == NULL ? NULL : model->footnote_id;
}

int footnote_association_additional_code_oplog_presenter_self_test(void)
{
    FootnoteAssociationAdditionalCodeOplogModel model = { .footnote_id = "id" };
    return strcmp(footnote_association_additional_code_oplog_presenter_resource_type(), "footnote_association_additional_code") == 0 &&
           strcmp(footnote_association_additional_code_oplog_presenter_id(&model), "id") == 0;
}
