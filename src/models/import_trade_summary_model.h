#ifndef TRADE_TARIFF_C_IMPORT_TRADE_SUMMARY_MODEL_H
#define TRADE_TARIFF_C_IMPORT_TRADE_SUMMARY_MODEL_H

typedef struct {
    char *id;
    char *basic_third_country_duty;
    char *preferential_tariff_duty;
    char *preferential_quota_duty;
} ImportTradeSummaryModel;

void import_trade_summary_model_free(ImportTradeSummaryModel *model);
int import_trade_summary_model_self_test(void);

#endif
