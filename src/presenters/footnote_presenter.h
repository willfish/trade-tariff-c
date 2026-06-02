#ifndef TRADE_TARIFF_C_FOOTNOTE_PRESENTER_H
#define TRADE_TARIFF_C_FOOTNOTE_PRESENTER_H

#include "models/footnote.h"

const char *footnote_presenter_resource_type(void);
const char *footnote_presenter_id(const Footnote *footnote);
const char *footnote_presenter_code(const Footnote *footnote);
char *footnote_presenter_formatted_description(const Footnote *footnote, const char *service);
int footnote_presenter_self_test(void);

#endif
