#include "presenters/classification_presenter.h"

#include <string.h>

const char *classification_section_presenter_resource_type(void)
{
    return "section";
}

const char *classification_section_presenter_id(const ClassificationSectionModel *model)
{
    return model == NULL ? NULL : model->id;
}

const char *classification_chapter_presenter_resource_type(void)
{
    return "chapter";
}

const char *classification_chapter_presenter_id(const ClassificationChapterModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

const char *classification_heading_presenter_resource_type(void)
{
    return "heading";
}

const char *classification_heading_presenter_id(const ClassificationHeadingModel *model)
{
    return model == NULL ? NULL : model->goods_nomenclature_sid;
}

int classification_presenter_self_test(void)
{
    ClassificationSectionModel section = { .id = "15" };
    ClassificationChapterModel chapter = { .goods_nomenclature_sid = "44810" };
    ClassificationHeadingModel heading = { .goods_nomenclature_sid = "45432" };

    return strcmp(classification_section_presenter_resource_type(), "section") == 0 &&
           strcmp(classification_section_presenter_id(&section), "15") == 0 &&
           classification_section_presenter_id(NULL) == NULL &&
           strcmp(classification_chapter_presenter_resource_type(), "chapter") == 0 &&
           strcmp(classification_chapter_presenter_id(&chapter), "44810") == 0 &&
           classification_chapter_presenter_id(NULL) == NULL &&
           strcmp(classification_heading_presenter_resource_type(), "heading") == 0 &&
           strcmp(classification_heading_presenter_id(&heading), "45432") == 0 &&
           classification_heading_presenter_id(NULL) == NULL;
}
