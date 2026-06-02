#ifndef TRADE_TARIFF_C_TRANSMISSION_COMMENT_OPLOG_MODEL_H
#define TRADE_TARIFF_C_TRANSMISSION_COMMENT_OPLOG_MODEL_H

#include <stddef.h>

typedef struct {
    char *comment_sid;
    char *language_id;
    char *comment_text;
    char *oid;
    char *operation;
    char *operation_date;
    char *filename;
} TransmissionCommentOplogModel;

typedef struct {
    TransmissionCommentOplogModel *items;
    size_t len;
    size_t cap;
} TransmissionCommentOplogModelList;

#endif
