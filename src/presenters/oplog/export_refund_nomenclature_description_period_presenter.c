#include "presenters/oplog/export_refund_nomenclature_description_period_presenter.h"

#include <string.h>

const char *export_refund_nomenclature_description_period_oplog_presenter_resource_type(void)
{
    return "export_refund_nomenclature_description_period";
}

const char *export_refund_nomenclature_description_period_oplog_presenter_id(const ExportRefundNomenclatureDescriptionPeriodOplogModel *model)
{
    return model == NULL ? NULL : model->export_refund_nomenclature_description_period_sid;
}

int export_refund_nomenclature_description_period_oplog_presenter_self_test(void)
{
    ExportRefundNomenclatureDescriptionPeriodOplogModel model = { .export_refund_nomenclature_description_period_sid = "id" };
    return strcmp(export_refund_nomenclature_description_period_oplog_presenter_resource_type(), "export_refund_nomenclature_description_period") == 0 &&
           strcmp(export_refund_nomenclature_description_period_oplog_presenter_id(&model), "id") == 0;
}
