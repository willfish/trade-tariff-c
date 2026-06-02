#ifndef TRADE_TARIFF_C_FOOTNOTE_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_FOOTNOTE_JSONAPI_RENDERER_H

#include "models/footnote.h"

#include <yyjson.h>

yyjson_mut_val *footnote_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                     const Footnote *footnote);
yyjson_mut_val *footnote_jsonapi_relationship_data_for_measure(yyjson_mut_doc *doc,
                                                               const FootnoteList *footnotes,
                                                               const char *measure_sid);
yyjson_mut_val *footnote_jsonapi_resource(yyjson_mut_doc *doc,
                                          const Footnote *footnote,
                                          const char *service);
int footnote_jsonapi_renderer_self_test(void);

#endif
