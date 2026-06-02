#include "models/classification_model.h"

#include <stdlib.h>
#include <string.h>

void classification_section_model_free(ClassificationSectionModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->id);
    free(model->numeral);
    free(model->title);
    free(model->position);
    free(model->section_note);
    *model = (ClassificationSectionModel){ 0 };
}

void classification_chapter_model_free(ClassificationChapterModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->goods_nomenclature_sid);
    free(model->goods_nomenclature_item_id);
    free(model->description);
    free(model->validity_start_date);
    free(model->validity_end_date);
    free(model->chapter_note);
    *model = (ClassificationChapterModel){ 0 };
}

void classification_heading_model_free(ClassificationHeadingModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->goods_nomenclature_sid);
    free(model->goods_nomenclature_item_id);
    free(model->description);
    free(model->validity_start_date);
    free(model->validity_end_date);
    *model = (ClassificationHeadingModel){ 0 };
}

void classification_model_free(ClassificationModel *model)
{
    if (model == NULL) {
        return;
    }

    classification_section_model_free(&model->section);
    classification_chapter_model_free(&model->chapter);
    classification_heading_model_free(&model->heading);
    *model = (ClassificationModel){ 0 };
}

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

int classification_model_self_test(void)
{
    ClassificationModel model = {
        .section = {
            .id = copy_literal("15"),
            .numeral = copy_literal("XV"),
            .title = copy_literal("Base metals"),
            .position = copy_literal("15"),
        },
        .chapter = {
            .goods_nomenclature_sid = copy_literal("44810"),
            .goods_nomenclature_item_id = copy_literal("7200000000"),
            .description = copy_literal("IRON AND STEEL"),
            .validity_start_date = copy_literal("1971-12-31T00:00:00.000Z"),
        },
        .heading = {
            .goods_nomenclature_sid = copy_literal("45432"),
            .goods_nomenclature_item_id = copy_literal("7212000000"),
            .description = copy_literal("Flat-rolled products"),
            .validity_start_date = copy_literal("1972-01-01T00:00:00.000Z"),
        },
        .has_section = 1,
        .has_chapter = 1,
        .has_heading = 1,
    };

    int ok = model.section.id != NULL &&
             model.chapter.goods_nomenclature_sid != NULL &&
             model.heading.goods_nomenclature_sid != NULL;
    classification_model_free(&model);
    return ok;
}
