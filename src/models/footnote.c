#include "models/footnote.h"

#include <stdlib.h>
#include <string.h>

void footnote_list_init(FootnoteList *list)
{
    if (list == NULL) {
        return;
    }
    list->items = NULL;
    list->len = 0;
    list->cap = 0;
}

int footnote_list_append(FootnoteList *list, Footnote footnote)
{
    if (list == NULL) {
        footnote_free(&footnote);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        Footnote *items = realloc(list->items, next_cap * sizeof(*items));
        if (items == NULL) {
            footnote_free(&footnote);
            return 0;
        }
        list->items = items;
        list->cap = next_cap;
    }

    list->items[list->len++] = footnote;
    return 1;
}

void footnote_free(Footnote *footnote)
{
    if (footnote == NULL) {
        return;
    }
    free(footnote->measure_sid);
    free(footnote->id);
    free(footnote->code);
    free(footnote->description);
    footnote->measure_sid = NULL;
    footnote->id = NULL;
    footnote->code = NULL;
    footnote->description = NULL;
}

void footnote_list_free(FootnoteList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        footnote_free(&list->items[i]);
    }
    free(list->items);
    footnote_list_init(list);
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

int footnote_self_test(void)
{
    FootnoteList list = { 0 };
    Footnote footnote = {
        .measure_sid = copy_literal("123"),
        .id = copy_literal("CD123"),
        .code = copy_literal("CD123"),
        .description = copy_literal("Footnote text"),
    };

    int ok = footnote.measure_sid != NULL &&
             footnote.id != NULL &&
             footnote.code != NULL &&
             footnote.description != NULL &&
             footnote_list_append(&list, footnote) &&
             list.len == 1 &&
             strcmp(list.items[0].measure_sid, "123") == 0;

    footnote_list_free(&list);
    return ok;
}
