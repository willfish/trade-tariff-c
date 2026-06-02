#ifndef TRADE_TARIFF_C_MEASURE_LEGAL_ACT_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_LEGAL_ACT_JSONAPI_RENDERER_H

#include "models/measure_legal_act_model.h"

#include <yyjson.h>

yyjson_mut_val *measure_legal_act_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const MeasureLegalActModel *legal_act);
yyjson_mut_val *measure_legal_act_jsonapi_relationship_data_for_measure(
    yyjson_mut_doc *doc,
    const MeasureLegalActModelList *legal_acts,
    const char *measure_sid);
yyjson_mut_val *measure_legal_act_jsonapi_suspension_relationship(
    yyjson_mut_doc *doc,
    const MeasureLegalActModelList *legal_acts,
    const char *measure_sid);
yyjson_mut_val *measure_legal_act_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const MeasureLegalActModel *legal_act,
                                                   const char *service);
yyjson_mut_val *measure_legal_act_jsonapi_suspension_resource(yyjson_mut_doc *doc,
                                                              const MeasureLegalActModel *legal_act);
int measure_legal_act_jsonapi_renderer_self_test(void);

#endif
