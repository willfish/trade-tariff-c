#include "presenters/oplog/export_refund_nomenclature_indent_presenter.h"

#include <string.h>

const char *export_refund_nomenclature_indent_oplog_presenter_resource_type(void)
{
    return "export_refund_nomenclature_indent";
}

const char *export_refund_nomenclature_indent_oplog_presenter_id(const ExportRefundNomenclatureIndentOplogModel *model)
{
    return model == NULL ? NULL : model->export_refund_nomenclature_indents_sid;
}

int export_refund_nomenclature_indent_oplog_presenter_self_test(void)
{
    ExportRefundNomenclatureIndentOplogModel model = { .export_refund_nomenclature_indents_sid = "id" };
    return strcmp(export_refund_nomenclature_indent_oplog_presenter_resource_type(), "export_refund_nomenclature_indent") == 0 &&
           strcmp(export_refund_nomenclature_indent_oplog_presenter_id(&model), "id") == 0;
}
