#ifndef TRADE_TARIFF_C_JSON_H
#define TRADE_TARIFF_C_JSON_H

char *json_escape(const char *input);
char *json_error(const char *title, const char *detail);
char *json_internal_error(void);

#endif
