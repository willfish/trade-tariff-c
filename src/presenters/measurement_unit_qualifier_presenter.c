#include "presenters/measurement_unit_qualifier_presenter.h"

#include "models/goods_nomenclature_description.h"
#include "presenters/goods_nomenclature_description_presenter.h"

#include <stdlib.h>
#include <string.h>

const char *measurement_unit_qualifier_presenter_resource_type(void)
{
    return "measurement_unit_qualifier";
}

const char *measurement_unit_qualifier_presenter_id(const MeasurementUnitQualifierModel *model)
{
    return model == NULL ? NULL : model->measurement_unit_qualifier_code;
}

char *measurement_unit_qualifier_presenter_formatted_description(const MeasurementUnitQualifierModel *model,
                                                                 const char *service)
{
    if (model == NULL) {
        return NULL;
    }

    GoodsNomenclatureDescriptionModel description = goods_nomenclature_description_new(model->description, service);
    return goods_nomenclature_description_presenter_formatted_description(&description);
}

int measurement_unit_qualifier_presenter_self_test(void)
{
    MeasurementUnitQualifierModel model = { .measurement_unit_qualifier_code = "X" };
    char *formatted = measurement_unit_qualifier_presenter_formatted_description(&model, "uk");
    int ok = strcmp(measurement_unit_qualifier_presenter_resource_type(), "measurement_unit_qualifier") == 0 &&
             strcmp(measurement_unit_qualifier_presenter_id(&model), "X") == 0 &&
             formatted != NULL &&
             strcmp(formatted, "") == 0;
    free(formatted);
    return ok;
}
