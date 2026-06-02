#ifndef TRADE_TARIFF_C_PUBLICATION_SIGLE_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_PUBLICATION_SIGLE_OPLOG_PRESENTER_H

#include "models/oplog/publication_sigle_model.h"

const char *publication_sigle_oplog_presenter_resource_type(void);
const char *publication_sigle_oplog_presenter_id(const PublicationSigleOplogModel *model);
int publication_sigle_oplog_presenter_self_test(void);

#endif
