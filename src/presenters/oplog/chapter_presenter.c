#include "presenters/oplog/chapter_presenter.h"

#include <string.h>

const char *chapter_oplog_presenter_resource_type(void)
{
    return "chapter";
}

const char *chapter_oplog_presenter_id(const ChapterOplogModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int chapter_oplog_presenter_self_test(void)
{
    ChapterOplogModel model = { .goods_nomenclature_sid = "id" };
    return strcmp(chapter_oplog_presenter_resource_type(), "chapter") == 0 &&
           strcmp(chapter_oplog_presenter_id(&model), "id") == 0;
}
