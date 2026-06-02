#include "presenters/oplog/footnote_association_meursing_heading_presenter.h"

#include <string.h>

const char *footnote_association_meursing_heading_oplog_presenter_resource_type(void)
{
    return "footnote_association_meursing_heading";
}

const char *footnote_association_meursing_heading_oplog_presenter_id(const FootnoteAssociationMeursingHeadingOplogModel *model)
{
    return model == NULL ? NULL : model->footnote_id;
}

int footnote_association_meursing_heading_oplog_presenter_self_test(void)
{
    FootnoteAssociationMeursingHeadingOplogModel model = { .footnote_id = "id" };
    return strcmp(footnote_association_meursing_heading_oplog_presenter_resource_type(), "footnote_association_meursing_heading") == 0 &&
           strcmp(footnote_association_meursing_heading_oplog_presenter_id(&model), "id") == 0;
}
