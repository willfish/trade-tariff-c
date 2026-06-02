#include "presenters/oplog/language_description_presenter.h"

#include <string.h>

const char *language_description_oplog_presenter_resource_type(void)
{
    return "language_description";
}

const char *language_description_oplog_presenter_id(const LanguageDescriptionOplogModel *model)
{
    return model == NULL ? NULL : model->language_code_id;
}

int language_description_oplog_presenter_self_test(void)
{
    LanguageDescriptionOplogModel model = { .language_code_id = "id" };
    return strcmp(language_description_oplog_presenter_resource_type(), "language_description") == 0 &&
           strcmp(language_description_oplog_presenter_id(&model), "id") == 0;
}
