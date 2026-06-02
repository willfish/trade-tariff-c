#ifndef TRADE_TARIFF_C_CLASSIFICATION_PRESENTER_H
#define TRADE_TARIFF_C_CLASSIFICATION_PRESENTER_H

#include "models/classification_model.h"

const char *classification_section_presenter_resource_type(void);
const char *classification_section_presenter_id(const ClassificationSectionModel *model);
const char *classification_chapter_presenter_resource_type(void);
const char *classification_chapter_presenter_id(const ClassificationChapterModel *model);
const char *classification_heading_presenter_resource_type(void);
const char *classification_heading_presenter_id(const ClassificationHeadingModel *model);
int classification_presenter_self_test(void);

#endif
