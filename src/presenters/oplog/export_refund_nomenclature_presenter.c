#include "presenters/oplog/export_refund_nomenclature_presenter.h"

#include <string.h>

const char *export_refund_nomenclature_oplog_presenter_resource_type(void)
{
    return "export_refund_nomenclature";
}

const char *export_refund_nomenclature_oplog_presenter_id(const ExportRefundNomenclatureOplogModel *model)
{
    return model == NULL ? NULL : model->export_refund_nomenclature_sid;
}

int export_refund_nomenclature_oplog_presenter_self_test(void)
{
    ExportRefundNomenclatureOplogModel model = { .export_refund_nomenclature_sid = "id" };
    return strcmp(export_refund_nomenclature_oplog_presenter_resource_type(), "export_refund_nomenclature") == 0 &&
           strcmp(export_refund_nomenclature_oplog_presenter_id(&model), "id") == 0;
}
