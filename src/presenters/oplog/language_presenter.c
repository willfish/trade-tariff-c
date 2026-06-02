#include "presenters/oplog/language_presenter.h"

#include <string.h>

const char *language_oplog_presenter_resource_type(void)
{
    return "language";
}

const char *language_oplog_presenter_id(const LanguageOplogModel *model)
{
    return model == NULL ? NULL : model->language_id;
}

int language_oplog_presenter_self_test(void)
{
    LanguageOplogModel model = { .language_id = "id" };
    return strcmp(language_oplog_presenter_resource_type(), "language") == 0 &&
           strcmp(language_oplog_presenter_id(&model), "id") == 0;
}
