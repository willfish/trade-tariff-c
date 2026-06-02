#ifndef TRADE_TARIFF_C_CLASSIFICATION_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_CLASSIFICATION_JSONAPI_RENDERER_H

#include "models/classification_model.h"
#include "models/guide_model.h"

#include <yyjson.h>

yyjson_mut_val *classification_section_jsonapi_resource(yyjson_mut_doc *doc,
                                                        const ClassificationSectionModel *section);
yyjson_mut_val *classification_chapter_jsonapi_resource(yyjson_mut_doc *doc,
                                                        const ClassificationChapterModel *chapter,
                                                        const GuideModelList *guides,
                                                        const char *service);
yyjson_mut_val *classification_heading_jsonapi_resource(yyjson_mut_doc *doc,
                                                        const ClassificationHeadingModel *heading,
                                                        const char *service);
int classification_jsonapi_renderer_self_test(void);

#endif
