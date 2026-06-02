#ifndef TRADE_TARIFF_C_APPENDIX_5A_MODEL_H
#define TRADE_TARIFF_C_APPENDIX_5A_MODEL_H

#include <stddef.h>

typedef struct {
    char *certificate_type_code;
    char *certificate_code;
    char *cds_guidance;
} Appendix5aModel;

typedef struct {
    Appendix5aModel *items;
    size_t len;
    size_t cap;
} Appendix5aModelList;

void appendix_5a_model_free(Appendix5aModel *model);
void appendix_5a_model_list_free(Appendix5aModelList *list);
int appendix_5a_model_list_append(Appendix5aModelList *list,
                                  Appendix5aModel model);
int appendix_5a_model_self_test(void);

#endif
