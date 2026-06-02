#include "presenters/oplog/publication_sigle_presenter.h"

#include <string.h>

const char *publication_sigle_oplog_presenter_resource_type(void)
{
    return "publication_sigle";
}

const char *publication_sigle_oplog_presenter_id(const PublicationSigleOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int publication_sigle_oplog_presenter_self_test(void)
{
    PublicationSigleOplogModel model = { .oid = "id" };
    return strcmp(publication_sigle_oplog_presenter_resource_type(), "publication_sigle") == 0 &&
           strcmp(publication_sigle_oplog_presenter_id(&model), "id") == 0;
}
