#include "presenters/oplog/certificate_presenter.h"

#include <string.h>

const char *certificate_oplog_presenter_resource_type(void)
{
    return "certificate";
}

const char *certificate_oplog_presenter_id(const CertificateOplogModel *model)
{
    return model == NULL ? NULL : model->certificate_code;
}

int certificate_oplog_presenter_self_test(void)
{
    CertificateOplogModel model = { .certificate_code = "id" };
    return strcmp(certificate_oplog_presenter_resource_type(), "certificate") == 0 &&
           strcmp(certificate_oplog_presenter_id(&model), "id") == 0;
}
