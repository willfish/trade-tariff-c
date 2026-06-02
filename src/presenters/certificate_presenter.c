#include "presenters/certificate_presenter.h"

#include <string.h>

const char *certificate_presenter_resource_type(void)
{
    return "certificate";
}

const char *certificate_presenter_id(const CertificateModel *model)
{
    return model == NULL ? NULL : model->certificate_code;
}

int certificate_presenter_self_test(void)
{
    CertificateModel model = { .certificate_code = "Y901" };
    return strcmp(certificate_presenter_resource_type(), "certificate") == 0 &&
           strcmp(certificate_presenter_id(&model), "Y901") == 0;
}
