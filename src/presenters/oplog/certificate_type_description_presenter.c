#include "presenters/oplog/certificate_type_description_presenter.h"

#include <string.h>

const char *certificate_type_description_oplog_presenter_resource_type(void)
{
    return "certificate_type_description";
}

const char *certificate_type_description_oplog_presenter_id(const CertificateTypeDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->language_id;
}

int certificate_type_description_oplog_presenter_self_test(void)
{
    CertificateTypeDescriptionOplogModel model = { .language_id = "id" };
    return strcmp(certificate_type_description_oplog_presenter_resource_type(), "certificate_type_description") == 0 &&
           strcmp(certificate_type_description_oplog_presenter_id(&model), "id") == 0;
}
