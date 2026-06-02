#include "presenters/oplog/certificate_description_presenter.h"

#include <string.h>

const char *certificate_description_oplog_presenter_resource_type(void)
{
    return "certificate_description";
}

const char *certificate_description_oplog_presenter_id(const CertificateDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->certificate_code;
}

int certificate_description_oplog_presenter_self_test(void)
{
    CertificateDescriptionOplogModel model = { .certificate_code = "id" };
    return strcmp(certificate_description_oplog_presenter_resource_type(), "certificate_description") == 0 &&
           strcmp(certificate_description_oplog_presenter_id(&model), "id") == 0;
}
