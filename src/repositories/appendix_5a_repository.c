#include "repositories/appendix_5a_repository.h"

#include "sql_loader.h"

#include <stdlib.h>
#include <string.h>

#define APPENDIX_5A_REPOSITORY_COLS 3

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

static char *owned_value(const DbRows *rows, int row, int col)
{
    if (db_rows_is_null(rows, row, col)) {
        return NULL;
    }

    const char *value = db_rows_value(rows, row, col);
    return value == NULL ? NULL : copy_literal(value);
}

char *appendix_5a_repository_document_code_array_param(const char *const *document_codes,
                                                       size_t document_code_count)
{
    if (document_codes == NULL || document_code_count == 0) {
        return copy_literal("{}");
    }

    size_t len = 3;
    size_t valid_count = 0;
    for (size_t i = 0; i < document_code_count; i++) {
        const char *document_code = document_codes[i];
        if (document_code == NULL || *document_code == '\0') {
            continue;
        }
        len += strlen(document_code) + 3;
        valid_count++;
    }

    if (valid_count == 0) {
        return copy_literal("{}");
    }

    char *param = malloc(len);
    if (param == NULL) {
        return NULL;
    }

    char *cursor = param;
    *cursor++ = '{';
    size_t written = 0;
    for (size_t i = 0; i < document_code_count; i++) {
        const char *document_code = document_codes[i];
        if (document_code == NULL || *document_code == '\0') {
            continue;
        }
        if (written > 0) {
            *cursor++ = ',';
        }
        *cursor++ = '"';
        size_t value_len = strlen(document_code);
        memcpy(cursor, document_code, value_len);
        cursor += value_len;
        *cursor++ = '"';
        written++;
    }
    *cursor++ = '}';
    *cursor = '\0';
    return param;
}

static char *build_sql(const CommodityRequestContext *context)
{
    if (context == NULL || context->schema == NULL) {
        return NULL;
    }

    char *template = sql_loader_load("sql/appendix_5as/for_document_codes.sql");
    if (template == NULL) {
        return NULL;
    }

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", context->schema },
    };
    char *sql = sql_loader_apply_bindings(template, bindings, sizeof(bindings) / sizeof(bindings[0]));
    free(template);
    return sql;
}

int appendix_5a_repository_hydrate_row(const DbRows *rows,
                                       int row,
                                       Appendix5aModel *model)
{
    if (rows == NULL || model == NULL || row < 0 || row >= rows->rows || rows->cols < APPENDIX_5A_REPOSITORY_COLS) {
        return 0;
    }

    *model = (Appendix5aModel){
        .certificate_type_code = owned_value(rows, row, 0),
        .certificate_code = owned_value(rows, row, 1),
        .cds_guidance = owned_value(rows, row, 2),
    };

    if (model->certificate_type_code == NULL || model->certificate_code == NULL) {
        appendix_5a_model_free(model);
        return 0;
    }

    return 1;
}

int appendix_5a_repository_fetch_for_document_codes(const CommodityRequestContext *context,
                                                    const char *const *document_codes,
                                                    size_t document_code_count,
                                                    Appendix5aModelList *appendix_5as)
{
    if (context == NULL || appendix_5as == NULL) {
        return 0;
    }

    char *document_code_param = appendix_5a_repository_document_code_array_param(document_codes, document_code_count);
    char *sql = build_sql(context);
    if (document_code_param == NULL || sql == NULL) {
        free(document_code_param);
        free(sql);
        return 0;
    }

    DbRows rows = db_fetch_rows("appendix_5as_for_document_codes", sql, document_code_param);
    free(document_code_param);
    free(sql);
    if (!rows.ok) {
        db_rows_free(&rows);
        return 0;
    }

    for (int row = 0; row < rows.rows; row++) {
        Appendix5aModel model = { 0 };
        if (!appendix_5a_repository_hydrate_row(&rows, row, &model)) {
            appendix_5a_model_list_free(appendix_5as);
            db_rows_free(&rows);
            return 0;
        }
        if (!appendix_5a_model_list_append(appendix_5as, model)) {
            appendix_5a_model_list_free(appendix_5as);
            db_rows_free(&rows);
            return 0;
        }
    }

    db_rows_free(&rows);
    return 1;
}

int appendix_5a_repository_self_test(void)
{
    const char *document_codes[] = { "C990", NULL, "", "Y123" };
    char *param = appendix_5a_repository_document_code_array_param(document_codes, sizeof(document_codes) / sizeof(document_codes[0]));
    int ok = param != NULL && strcmp(param, "{\"C990\",\"Y123\"}") == 0;
    free(param);

    CommodityRequestContext context = {
        .schema = "uk",
    };
    char *sql = build_sql(&context);
    ok = ok && sql != NULL &&
         strstr(sql, "uk.appendix_5as") != NULL &&
         strstr(sql, "{{SCHEMA}}") == NULL;
    free(sql);

    return ok;
}
