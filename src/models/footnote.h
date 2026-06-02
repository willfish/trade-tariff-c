#ifndef TRADE_TARIFF_C_FOOTNOTE_MODEL_H
#define TRADE_TARIFF_C_FOOTNOTE_MODEL_H

#include <stddef.h>

typedef struct {
    char *measure_sid;
    char *id;
    char *code;
    char *description;
} Footnote;

typedef struct {
    Footnote *items;
    size_t len;
    size_t cap;
} FootnoteList;

void footnote_list_init(FootnoteList *list);
int footnote_list_append(FootnoteList *list, Footnote footnote);
void footnote_free(Footnote *footnote);
void footnote_list_free(FootnoteList *list);
int footnote_self_test(void);

#endif
