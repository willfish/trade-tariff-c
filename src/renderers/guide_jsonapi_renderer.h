#ifndef TRADE_TARIFF_C_GUIDE_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_GUIDE_JSONAPI_RENDERER_H

#include "models/guide_model.h"

#include <yyjson.h>

yyjson_mut_val *guide_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                  const GuideModel *guide);
yyjson_mut_val *guide_jsonapi_relationship_data(yyjson_mut_doc *doc,
                                                const GuideModelList *guides);
yyjson_mut_val *guide_jsonapi_resource(yyjson_mut_doc *doc,
                                       const GuideModel *guide);
int guide_jsonapi_renderer_self_test(void);

#endif
