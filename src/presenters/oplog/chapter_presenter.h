#ifndef TRADE_TARIFF_C_CHAPTER_OPLOG_PRESENTER_H
#define TRADE_TARIFF_C_CHAPTER_OPLOG_PRESENTER_H

#include "models/oplog/chapter_model.h"

const char *chapter_oplog_presenter_resource_type(void);
const char *chapter_oplog_presenter_id(const ChapterOplogModel *model);
int chapter_oplog_presenter_self_test(void);

#endif
