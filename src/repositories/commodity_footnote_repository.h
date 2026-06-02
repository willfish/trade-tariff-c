#ifndef TRADE_TARIFF_C_COMMODITY_FOOTNOTE_REPOSITORY_H
#define TRADE_TARIFF_C_COMMODITY_FOOTNOTE_REPOSITORY_H

#include "db.h"
#include "models/footnote.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *commodity_footnote_repository_sid_array_param(const char *const *goods_nomenclature_sids,
                                                    size_t sid_count);
int commodity_footnote_repository_hydrate_row(const DbRows *rows,
                                              int row,
                                              Footnote *footnote);
int commodity_footnote_repository_fetch_for_goods_nomenclature_sids(const CommodityRequestContext *context,
                                                                    const char *const *goods_nomenclature_sids,
                                                                    size_t sid_count,
                                                                    FootnoteList *footnotes);
int commodity_footnote_repository_self_test(void);

#endif
