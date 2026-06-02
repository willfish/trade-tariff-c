#include "presenters/footnote_presenter.h"

#include "formatters/description_formatter.h"

#include <stdlib.h>
#include <string.h>

const char *footnote_presenter_resource_type(void)
{
    return "footnote";
}

const char *footnote_presenter_id(const Footnote *footnote)
{
    return footnote == NULL ? NULL : footnote->id;
}

const char *footnote_presenter_code(const Footnote *footnote)
{
    return footnote == NULL ? NULL : footnote->code;
}

char *footnote_presenter_formatted_description(const Footnote *footnote, const char *service)
{
    int uk = service == NULL || strcmp(service, "xi") != 0;
    return description_formatter_format(footnote == NULL ? NULL : footnote->description, uk);
}

int footnote_presenter_self_test(void)
{
    Footnote footnote = {
        .id = "DS525",
        .code = "DS525",
        .description = "<p>x</p><br><ul><br>  <li>1,2 &apos;</li><br></ul>",
    };
    char *description = footnote_presenter_formatted_description(&footnote, "uk");
    if (description == NULL) {
        return 0;
    }
    int ok = strcmp(footnote_presenter_resource_type(), "footnote") == 0 &&
             strcmp(footnote_presenter_id(&footnote), "DS525") == 0 &&
             strcmp(footnote_presenter_code(&footnote), "DS525") == 0 &&
             footnote_presenter_id(NULL) == NULL &&
             footnote_presenter_code(NULL) == NULL &&
             strcmp(description, "<p>x</p><br><ul><li>1.2 &amp;apos;</li></ul>") == 0;
    free(description);
    return ok;
}
