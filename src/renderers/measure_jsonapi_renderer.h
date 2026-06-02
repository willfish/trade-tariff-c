#ifndef TRADE_TARIFF_C_MEASURE_JSONAPI_RENDERER_H
#define TRADE_TARIFF_C_MEASURE_JSONAPI_RENDERER_H

#include "models/appendix_5a_model.h"
#include "models/measure_component_model.h"
#include "models/measure_condition_component_model.h"
#include "models/measure_condition_model.h"
#include "models/measure_condition_permutation_model.h"
#include "models/geographical_area_model.h"
#include "models/measure_excluded_geographical_area_model.h"
#include "models/footnote.h"
#include "models/measure_legal_act_model.h"
#include "models/measure_model.h"
#include "presenters/preference_code_presenter.h"

#include <yyjson.h>

yyjson_mut_val *measure_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                    const MeasureModel *measure);
yyjson_mut_val *measure_jsonapi_relationship_data(yyjson_mut_doc *doc,
                                                  const MeasureModelList *measures,
                                                  int import_relationship);
yyjson_mut_val *measure_jsonapi_resource(yyjson_mut_doc *doc,
                                         const MeasureModel *measure,
                                         const PreferenceCodeDeclarable *preference_declarable,
                                         const MeasureComponentModelList *components,
                                         const MeasureComponentModelList *resolved_components,
                                         const MeasureConditionModelList *conditions,
                                         const MeasureConditionPermutationGroupModelList *condition_permutation_groups,
                                         const MeasureConditionComponentModelList *condition_components,
                                         const Appendix5aModelList *appendix_5as,
                                         const GeographicalAreaModelList *geographical_areas,
                                         const MeasureExcludedGeographicalAreaModelList *excluded_geographical_areas,
                                         const FootnoteList *footnotes,
                                         const MeasureLegalActModelList *legal_acts);
int measure_jsonapi_renderer_self_test(void);

#endif
