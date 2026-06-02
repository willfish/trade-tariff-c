#ifndef TRADE_TARIFF_C_MEASURE_FOOTNOTE_REPOSITORY_H
#define TRADE_TARIFF_C_MEASURE_FOOTNOTE_REPOSITORY_H

#include "db.h"
#include "models/footnote.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *measure_footnote_repository_sid_array_param(const char *const *measure_sids,
                                                  size_t sid_count);
int measure_footnote_repository_hydrate_row(const DbRows *rows,
                                            int row,
                                            Footnote *footnote);
int measure_footnote_repository_fetch_for_measure_sids(const CommodityRequestContext *context,
                                                       const char *const *measure_sids,
                                                       size_t sid_count,
                                                       FootnoteList *footnotes);
int measure_footnote_repository_self_test(void);

#endif
