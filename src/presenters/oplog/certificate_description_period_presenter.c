#include "presenters/oplog/certificate_description_period_presenter.h"

#include <string.h>

const char *certificate_description_period_oplog_presenter_resource_type(void)
{
    return "certificate_description_period";
}

const char *certificate_description_period_oplog_presenter_id(const CertificateDescriptionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->certificate_description_period_sid;
}

int certificate_description_period_oplog_presenter_self_test(void)
{
    CertificateDescriptionPeriodOplogModel model = { .certificate_description_period_sid = "id" };
    return strcmp(certificate_description_period_oplog_presenter_resource_type(), "certificate_description_period") == 0 &&
           strcmp(certificate_description_period_oplog_presenter_id(&model), "id") == 0;
}
