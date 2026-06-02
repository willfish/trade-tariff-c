#include "models/appendix_5a_model.h"

#include <stdlib.h>
#include <string.h>

void appendix_5a_model_free(Appendix5aModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->certificate_type_code);
    free(model->certificate_code);
    free(model->cds_guidance);
    *model = (Appendix5aModel){ 0 };
}

void appendix_5a_model_list_free(Appendix5aModelList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        appendix_5a_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (Appendix5aModelList){ 0 };
}

int appendix_5a_model_list_append(Appendix5aModelList *list,
                                  Appendix5aModel model)
{
    if (list == NULL) {
        appendix_5a_model_free(&model);
        return 0;
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        Appendix5aModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            appendix_5a_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }
    list->items[list->len++] = model;
    return 1;
}

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

int appendix_5a_model_self_test(void)
{
    Appendix5aModelList list = { 0 };
    Appendix5aModel model = {
        .certificate_type_code = copy_literal("C"),
        .certificate_code = copy_literal("990"),
        .cds_guidance = copy_literal("Guidance"),
    };
    int ok = model.certificate_type_code != NULL &&
             model.certificate_code != NULL &&
             model.cds_guidance != NULL &&
             appendix_5a_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].certificate_type_code, "C") == 0;
    appendix_5a_model_list_free(&list);
    return ok;
}
