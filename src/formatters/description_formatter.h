#ifndef TRADE_TARIFF_C_DESCRIPTION_FORMATTER_H
#define TRADE_TARIFF_C_DESCRIPTION_FORMATTER_H

char *description_formatter_format(const char *description, int uk);
char *description_formatter_normalize_goods_description(const char *description);
char *description_formatter_trim_plain(const char *description);
int description_formatter_self_test(void);

#endif
