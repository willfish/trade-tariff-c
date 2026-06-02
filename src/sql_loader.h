#ifndef TRADE_TARIFF_C_SQL_LOADER_H
#define TRADE_TARIFF_C_SQL_LOADER_H

#include <stddef.h>

typedef struct {
    const char *name;
    const char *value;
} SqlTemplateBinding;

char *sql_loader_load(const char *relative_path);
char *sql_loader_apply_bindings(const char *template_sql,
                                const SqlTemplateBinding *bindings,
                                size_t binding_count);
int sql_loader_self_test(void);

#endif
