#ifndef TRADE_TARIFF_C_OPLOG_TABLE_REPOSITORY_H
#define TRADE_TARIFF_C_OPLOG_TABLE_REPOSITORY_H

#include "db.h"

typedef struct {
    const char *schema;
    const char *table;
    const char *columns;
    const char *where_sql;
    const char *order_sql;
} OplogTableQuery;

char *oplog_table_repository_build_select(const OplogTableQuery *query);
DbRows oplog_table_repository_fetch(const char *statement_name,
                                    const OplogTableQuery *query,
                                    const char *param);
int oplog_table_repository_self_test(void);

#endif
