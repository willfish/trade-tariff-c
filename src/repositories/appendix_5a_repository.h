#ifndef TRADE_TARIFF_C_APPENDIX_5A_REPOSITORY_H
#define TRADE_TARIFF_C_APPENDIX_5A_REPOSITORY_H

#include "db.h"
#include "models/appendix_5a_model.h"
#include "request/commodity_request_context.h"

#include <stddef.h>

char *appendix_5a_repository_document_code_array_param(const char *const *document_codes,
                                                       size_t document_code_count);
int appendix_5a_repository_hydrate_row(const DbRows *rows,
                                       int row,
                                       Appendix5aModel *model);
int appendix_5a_repository_fetch_for_document_codes(const CommodityRequestContext *context,
                                                    const char *const *document_codes,
                                                    size_t document_code_count,
                                                    Appendix5aModelList *appendix_5as);
int appendix_5a_repository_self_test(void);

#endif
