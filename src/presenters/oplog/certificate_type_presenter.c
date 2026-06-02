#include "presenters/oplog/certificate_type_presenter.h"

#include <string.h>

const char *certificate_type_oplog_presenter_resource_type(void)
{
    return "certificate_type";
}

const char *certificate_type_oplog_presenter_id(const CertificateTypeOplogModel *model)
{
    return model == NULL ? NULL : model->oid;
}

int certificate_type_oplog_presenter_self_test(void)
{
    CertificateTypeOplogModel model = { .oid = "id" };
    return strcmp(certificate_type_oplog_presenter_resource_type(), "certificate_type") == 0 &&
           strcmp(certificate_type_oplog_presenter_id(&model), "id") == 0;
}
