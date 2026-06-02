#include "presenters/regulation_presenter.h"

#include <string.h>

const char *regulation_presenter_resource_type(void)
{
    return "legal_act";
}

const char *regulation_presenter_id(const RegulationModel *model)
{
    return model == NULL ? NULL : model->regulation_id;
}

int regulation_presenter_self_test(void)
{
    RegulationModel model = { .regulation_id = "R123" };
    return strcmp(regulation_presenter_resource_type(), "legal_act") == 0 &&
           strcmp(regulation_presenter_id(&model), "R123") == 0;
}
