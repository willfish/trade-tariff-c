#include "presenters/guide_presenter.h"

#include <string.h>

const char *guide_presenter_resource_type(void)
{
    return "guide";
}

const char *guide_presenter_id(const GuideModel *guide)
{
    return guide == NULL ? NULL : guide->id;
}

int guide_presenter_self_test(void)
{
    GuideModel guide = {
        .id = "12",
        .title = "Using a commodity code",
        .url = "https://www.gov.uk/guidance/using-a-commodity-code",
    };

    return strcmp(guide_presenter_resource_type(), "guide") == 0 &&
           strcmp(guide_presenter_id(&guide), "12") == 0 &&
           guide_presenter_id(NULL) == NULL;
}
