#ifndef TRADE_TARIFF_C_CLASSIFICATION_MODEL_H
#define TRADE_TARIFF_C_CLASSIFICATION_MODEL_H

typedef struct {
    char *id;
    char *numeral;
    char *title;
    char *position;
    char *section_note;
} ClassificationSectionModel;

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *description;
    char *validity_start_date;
    char *validity_end_date;
    char *chapter_note;
} ClassificationChapterModel;

typedef struct {
    char *goods_nomenclature_sid;
    char *goods_nomenclature_item_id;
    char *description;
    char *validity_start_date;
    char *validity_end_date;
} ClassificationHeadingModel;

typedef struct {
    ClassificationSectionModel section;
    ClassificationChapterModel chapter;
    ClassificationHeadingModel heading;
    int has_section;
    int has_chapter;
    int has_heading;
} ClassificationModel;

void classification_section_model_free(ClassificationSectionModel *model);
void classification_chapter_model_free(ClassificationChapterModel *model);
void classification_heading_model_free(ClassificationHeadingModel *model);
void classification_model_free(ClassificationModel *model);
int classification_model_self_test(void);

#endif
