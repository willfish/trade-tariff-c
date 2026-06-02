#include "formatters/description_formatter.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} StringBuffer;

static int buffer_reserve(StringBuffer *buffer, size_t extra)
{
    if (buffer->len + extra + 1 <= buffer->cap) {
        return 1;
    }

    size_t cap = buffer->cap == 0 ? 128 : buffer->cap;
    while (buffer->len + extra + 1 > cap) {
        cap *= 2;
    }

    char *data = realloc(buffer->data, cap);
    if (data == NULL) {
        return 0;
    }
    buffer->data = data;
    buffer->cap = cap;
    return 1;
}

static int buffer_append_len(StringBuffer *buffer, const char *text, size_t len)
{
    if (!buffer_reserve(buffer, len)) {
        return 0;
    }
    memcpy(buffer->data + buffer->len, text, len);
    buffer->len += len;
    buffer->data[buffer->len] = '\0';
    return 1;
}

static int buffer_append(StringBuffer *buffer, const char *text)
{
    return buffer_append_len(buffer, text, strlen(text));
}

static char *buffer_take(StringBuffer *buffer)
{
    if (buffer->data == NULL) {
        return strdup("");
    }
    return buffer->data;
}

static size_t count_occurrences(const char *input, const char *needle)
{
    size_t count = 0;
    size_t needle_len = strlen(needle);
    const char *cursor = input;
    while ((cursor = strstr(cursor, needle)) != NULL) {
        count++;
        cursor += needle_len;
    }
    return count;
}

static char *replace_all_owned(char *input, const char *needle, const char *replacement)
{
    size_t count = count_occurrences(input, needle);
    if (count == 0) {
        return input;
    }

    size_t input_len = strlen(input);
    size_t needle_len = strlen(needle);
    size_t replacement_len = strlen(replacement);
    char *output = malloc(input_len + count * (replacement_len - needle_len) + 1);
    if (output == NULL) {
        free(input);
        return NULL;
    }

    char *out = output;
    const char *cursor = input;
    const char *match;
    while ((match = strstr(cursor, needle)) != NULL) {
        size_t prefix_len = (size_t)(match - cursor);
        memcpy(out, cursor, prefix_len);
        out += prefix_len;
        memcpy(out, replacement, replacement_len);
        out += replacement_len;
        cursor = match + needle_len;
    }
    strcpy(out, cursor);
    free(input);
    return output;
}

static int ascii_starts_case_insensitive(const char *text, const char *needle)
{
    for (size_t i = 0; needle[i] != '\0'; i++) {
        if (text[i] == '\0') {
            return 0;
        }
        if (tolower((unsigned char)text[i]) != tolower((unsigned char)needle[i])) {
            return 0;
        }
    }
    return 1;
}

static char *replace_all_case_insensitive_owned(char *input, const char *needle, const char *replacement)
{
    size_t needle_len = strlen(needle);
    StringBuffer out = { 0 };

    for (const char *cursor = input; *cursor != '\0';) {
        if (ascii_starts_case_insensitive(cursor, needle)) {
            if (!buffer_append(&out, replacement)) {
                free(out.data);
                free(input);
                return NULL;
            }
            cursor += needle_len;
        } else {
            if (!buffer_append_len(&out, cursor, 1)) {
                free(out.data);
                free(input);
                return NULL;
            }
            cursor++;
        }
    }

    free(input);
    return buffer_take(&out);
}

static char *remove_spaces_before_percent_owned(char *input)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0'; p++) {
        if (isdigit((unsigned char)*p)) {
            const char *space = p + 1;
            while (*space == ' ' || *space == '\t' || *space == '\n' || *space == '\r') {
                space++;
            }
            if (*space == '%' && space != p + 1) {
                if (!buffer_append_len(&out, p, 1) || !buffer_append_len(&out, "%", 1)) {
                    free(out.data);
                    free(input);
                    return NULL;
                }
                p = space;
                continue;
            }
        }
        if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
    }

    free(input);
    return buffer_take(&out);
}

static int ampersand_is_preserved_entity(const char *p)
{
    return p[0] == '&' && (p[1] == '#' || strncmp(p + 1, "nbsp", 4) == 0);
}

static char *escape_ampersands_owned(char *input)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0'; p++) {
        if (*p == '&' && !ampersand_is_preserved_entity(p)) {
            if (!buffer_append(&out, "&amp;")) {
                free(out.data);
                free(input);
                return NULL;
            }
        } else if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
    }

    free(input);
    return buffer_take(&out);
}

static char *decimal_comma_owned(char *input)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0'; p++) {
        if (isdigit((unsigned char)p[0]) && p[1] == ',' && isdigit((unsigned char)p[2])) {
            if (!buffer_append_len(&out, p, 1) ||
                !buffer_append_len(&out, ".", 1) ||
                !buffer_append_len(&out, p + 2, 1)) {
                free(out.data);
                free(input);
                return NULL;
            }
            p += 2;
        } else if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
    }

    free(input);
    return buffer_take(&out);
}

static char *tag_marker_owned(char *input, char marker, const char *open, const char *close)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0'; p++) {
        if (*p == marker && p[1] != '\0') {
            if (!buffer_append(&out, open) ||
                !buffer_append_len(&out, p + 1, 1) ||
                !buffer_append(&out, close)) {
                free(out.data);
                free(input);
                return NULL;
            }
            p++;
        } else if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
    }

    free(input);
    return buffer_take(&out);
}

static char *trim_marker_owned(char *input, char marker)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0'; p++) {
        if (*p == marker && p[1] != '\0') {
            if (!buffer_append_len(&out, p + 1, 1)) {
                free(out.data);
                free(input);
                return NULL;
            }
            p++;
        } else if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
    }

    free(input);
    return buffer_take(&out);
}

static char *sub_tag_back_to_marker_owned(char *input)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0';) {
        if (ascii_starts_case_insensitive(p, "<sub>") &&
            isalpha((unsigned char)p[5]) &&
            ascii_starts_case_insensitive(p + 6, "</sub>")) {
            if (!buffer_append_len(&out, "@", 1) ||
                !buffer_append_len(&out, p + 5, 1)) {
                free(out.data);
                free(input);
                return NULL;
            }
            p += 12;
        } else {
            if (!buffer_append_len(&out, p, 1)) {
                free(out.data);
                free(input);
                return NULL;
            }
            p++;
        }
    }

    free(input);
    return buffer_take(&out);
}

static int html_space(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

static char *remove_break_before_tag_owned(char *input, const char *tag)
{
    StringBuffer out = { 0 };
    size_t tag_len = strlen(tag);

    for (const char *p = input; *p != '\0';) {
        if (strncmp(p, "<br>", 4) == 0) {
            const char *after_break = p + 4;
            const char *after_space = after_break;
            while (html_space(*after_space)) {
                after_space++;
            }
            if (strncmp(after_space, tag, tag_len) == 0) {
                if (!buffer_append_len(&out, tag, tag_len)) {
                    free(out.data);
                    free(input);
                    return NULL;
                }
                p = after_space + tag_len;
                continue;
            }
        }
        if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
        p++;
    }

    free(input);
    return buffer_take(&out);
}

static char *collapse_three_or_more_breaks_owned(char *input)
{
    StringBuffer out = { 0 };

    for (const char *p = input; *p != '\0';) {
        if (strncmp(p, "<br>", 4) == 0) {
            const char *run = p;
            size_t count = 0;
            while (strncmp(run, "<br>", 4) == 0) {
                count++;
                run += 4;
            }
            size_t keep = count >= 3 ? 2 : count;
            for (size_t i = 0; i < keep; i++) {
                if (!buffer_append(&out, "<br>")) {
                    free(out.data);
                    free(input);
                    return NULL;
                }
            }
            p = run;
            continue;
        }
        if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            free(input);
            return NULL;
        }
        p++;
    }

    free(input);
    return buffer_take(&out);
}

static const char *consume_goods_description_break_unit(const char *p)
{
    if (*p == ' ') {
        p++;
    }
    if (strncmp(p, "<br>", 4) != 0) {
        return NULL;
    }
    p += 4;
    if (*p == ' ') {
        p++;
    }
    return p;
}

char *description_formatter_normalize_goods_description(const char *description)
{
    if (description == NULL || description[0] == '\0') {
        return strdup("");
    }

    StringBuffer out = { 0 };
    for (const char *p = description; *p != '\0';) {
        const char *run = p;
        size_t count = 0;
        const char *next = consume_goods_description_break_unit(run);
        while (next != NULL) {
            count++;
            run = next;
            next = consume_goods_description_break_unit(run);
        }

        if (count >= 2) {
            if (!buffer_append(&out, "<br>")) {
                free(out.data);
                return NULL;
            }
            p = run;
            continue;
        }

        if (!buffer_append_len(&out, p, 1)) {
            free(out.data);
            return NULL;
        }
        p++;
    }

    return buffer_take(&out);
}

char *description_formatter_format(const char *description, int uk)
{
    if (description == NULL || description[0] == '\0') {
        return strdup("");
    }

    char *out = strdup(description);
    if (out == NULL) {
        return NULL;
    }

    out = replace_all_owned(out, "|%", "%");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "\xC2\xA0", " ");
    if (out == NULL) return NULL;
    out = remove_spaces_before_percent_owned(out);
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "-|", "\n-");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "|", "&nbsp;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!1!", "<br />");
    if (out == NULL) return NULL;
    out = escape_ampersands_owned(out);
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!X!", "&times;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!x!", "&times;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!o!", "&deg;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!O!", "&deg;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!>=!", "&ge;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!<=!", "&le;");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "\n\n", "<br/>");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "\n", "<br/>");
    if (out == NULL) return NULL;

    if (uk) {
        out = replace_all_case_insensitive_owned(out, "izing", "ising");
        if (out == NULL) return NULL;
        out = replace_all_case_insensitive_owned(out, "ization", "isation");
        if (out == NULL) return NULL;
        out = replace_all_case_insensitive_owned(out, "ized", "ised");
        if (out == NULL) return NULL;
        out = decimal_comma_owned(out);
        if (out == NULL) return NULL;
    }

    out = tag_marker_owned(out, '@', "<sub>", "</sub>");
    if (out == NULL) return NULL;
    out = tag_marker_owned(out, '$', "<sup>", "</sup>");
    if (out == NULL) return NULL;
    out = sub_tag_back_to_marker_owned(out);
    if (out == NULL) return NULL;
    out = remove_break_before_tag_owned(out, "<li>");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "<br><br><ul>", "<ul>");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "<br></ul><br>", "</ul>");
    if (out == NULL) return NULL;
    out = remove_break_before_tag_owned(out, "</ul>");
    if (out == NULL) return NULL;
    out = collapse_three_or_more_breaks_owned(out);
    if (out == NULL) return NULL;

    return out;
}

char *description_formatter_trim_plain(const char *description)
{
    if (description == NULL || description[0] == '\0') {
        return strdup("");
    }

    char *out = strdup(description);
    if (out == NULL) {
        return NULL;
    }

    out = replace_all_owned(out, "&nbsp;", " ");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "|", " ");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!1!", "");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!X!", "");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!x!", "");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!o!", "");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!O!", "");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!>=!", "");
    if (out == NULL) return NULL;
    out = replace_all_owned(out, "!<=!", "");
    if (out == NULL) return NULL;
    out = trim_marker_owned(out, '@');
    if (out == NULL) return NULL;
    out = trim_marker_owned(out, '$');
    if (out == NULL) return NULL;

    return out;
}

int description_formatter_self_test(void)
{
    char *formatted = description_formatter_format("A<br><br><br><ul><br>  <li>x &apos; y</li><br></ul>", 1);
    if (formatted == NULL) {
        return 0;
    }
    char *normalized = description_formatter_normalize_goods_description(
        "Colour negative film:<br>                            <br><br>-\xC2\xA0of a width");
    char *trimmed = description_formatter_trim_plain("10$-<sup>6</sup>");
    int ok = normalized != NULL &&
             trimmed != NULL &&
             strcmp(formatted, "A<br><ul><li>x &amp;apos; y</li></ul>") == 0 &&
             strcmp(normalized, "Colour negative film:<br>                           <br>-\xC2\xA0of a width") == 0 &&
             strcmp(trimmed, "10-<sup>6</sup>") == 0;
    free(formatted);
    free(normalized);
    free(trimmed);
    return ok;
}
