#include "presenters/oplog/transmission_comment_presenter.h"

#include <string.h>

const char *transmission_comment_oplog_presenter_resource_type(void)
{
    return "transmission_comment";
}

const char *transmission_comment_oplog_presenter_id(const TransmissionCommentOplogModel *model)
{
    return model == NULL ? NULL : model->comment_sid;
}

int transmission_comment_oplog_presenter_self_test(void)
{
    TransmissionCommentOplogModel model = { .comment_sid = "id" };
    return strcmp(transmission_comment_oplog_presenter_resource_type(), "transmission_comment") == 0 &&
           strcmp(transmission_comment_oplog_presenter_id(&model), "id") == 0;
}
