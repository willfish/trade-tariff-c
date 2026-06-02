#include "renderers/classification_jsonapi_renderer.h"

#include "models/goods_nomenclature_description.h"
#include "presenters/classification_presenter.h"
#include "presenters/goods_nomenclature_description_presenter.h"
#include "renderers/guide_jsonapi_renderer.h"
#include "renderers/jsonapi_relationship.h"

#include <stdlib.h>

static int add_string_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
}

static char *presented_description(const char *description, const char *service)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(description, service);
    return goods_nomenclature_description_presenter_description(&model);
}

static char *presented_description_plain(const char *description, const char *service)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(description, service);
    return goods_nomenclature_description_presenter_description_plain(&model);
}

static char *presented_formatted_description(const char *description, const char *service)
{
    GoodsNomenclatureDescriptionModel model = goods_nomenclature_description_new(description, service);
    return goods_nomenclature_description_presenter_formatted_description(&model);
}

yyjson_mut_val *classification_section_jsonapi_resource(yyjson_mut_doc *doc,
                                                        const ClassificationSectionModel *section)
{
    const char *id = classification_section_presenter_id(section);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       classification_section_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    int position = section->position == NULL ? 0 : atoi(section->position);
    if (resource == NULL ||
        attributes == NULL ||
        add_string_or_null(doc, attributes, "numeral", section->numeral) == 0 ||
        add_string_or_null(doc, attributes, "title", section->title) == 0 ||
        !yyjson_mut_obj_add_int(doc, attributes, "position", position)) {
        return NULL;
    }

    if (section->section_note != NULL &&
        !yyjson_mut_obj_add_strcpy(doc, attributes, "section_note", section->section_note)) {
        return NULL;
    }

    return yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ? resource : NULL;
}

yyjson_mut_val *classification_chapter_jsonapi_resource(yyjson_mut_doc *doc,
                                                        const ClassificationChapterModel *chapter,
                                                        const GuideModelList *guides,
                                                        const char *service)
{
    const char *id = classification_chapter_presenter_id(chapter);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       classification_chapter_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *description = presented_description(chapter->description, service);
    char *formatted_description = presented_formatted_description(chapter->description, service);
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *guide_rel = yyjson_mut_obj(doc);
    yyjson_mut_val *guide_data = guide_jsonapi_relationship_data(doc, guides);
    if (resource == NULL ||
        attributes == NULL ||
        relationships == NULL ||
        guide_rel == NULL ||
        guide_data == NULL ||
        description == NULL ||
        formatted_description == NULL ||
        add_string_or_null(doc, attributes, "goods_nomenclature_item_id", chapter->goods_nomenclature_item_id) == 0 ||
        add_string_or_null(doc, attributes, "description", description) == 0 ||
        add_string_or_null(doc, attributes, "formatted_description", formatted_description) == 0 ||
        add_string_or_null(doc, attributes, "validity_start_date", chapter->validity_start_date) == 0 ||
        add_string_or_null(doc, attributes, "validity_end_date", chapter->validity_end_date) == 0) {
        free(description);
        free(formatted_description);
        return NULL;
    }

    if (chapter->chapter_note != NULL &&
        !yyjson_mut_obj_add_strcpy(doc, attributes, "chapter_note", chapter->chapter_note)) {
        free(description);
        free(formatted_description);
        return NULL;
    }

    free(description);
    free(formatted_description);
    return yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) &&
                   yyjson_mut_obj_add_val(doc, guide_rel, "data", guide_data) &&
                   yyjson_mut_obj_add_val(doc, relationships, "guides", guide_rel) &&
                   yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)
               ? resource
               : NULL;
}

yyjson_mut_val *classification_heading_jsonapi_resource(yyjson_mut_doc *doc,
                                                        const ClassificationHeadingModel *heading,
                                                        const char *service)
{
    const char *id = classification_heading_presenter_id(heading);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       classification_heading_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *description = presented_description(heading->description, service);
    char *description_plain = presented_description_plain(heading->description, service);
    char *formatted_description = presented_formatted_description(heading->description, service);
    if (resource == NULL ||
        attributes == NULL ||
        description == NULL ||
        description_plain == NULL ||
        formatted_description == NULL ||
        add_string_or_null(doc, attributes, "goods_nomenclature_item_id", heading->goods_nomenclature_item_id) == 0 ||
        add_string_or_null(doc, attributes, "description", description) == 0 ||
        add_string_or_null(doc, attributes, "formatted_description", formatted_description) == 0 ||
        add_string_or_null(doc, attributes, "description_plain", description_plain) == 0 ||
        add_string_or_null(doc, attributes, "validity_start_date", heading->validity_start_date) == 0 ||
        add_string_or_null(doc, attributes, "validity_end_date", heading->validity_end_date) == 0) {
        free(description);
        free(description_plain);
        free(formatted_description);
        return NULL;
    }

    free(description);
    free(description_plain);
    free(formatted_description);
    return yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ? resource : NULL;
}

int classification_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    ClassificationSectionModel section = {
        .id = "15",
        .numeral = "XV",
        .title = "Base metals",
        .position = "15",
    };
    ClassificationChapterModel chapter = {
        .goods_nomenclature_sid = "44810",
        .goods_nomenclature_item_id = "7200000000",
        .description = "IRON AND STEEL",
        .validity_start_date = "1971-12-31T00:00:00.000Z",
    };
    ClassificationHeadingModel heading = {
        .goods_nomenclature_sid = "45432",
        .goods_nomenclature_item_id = "7212000000",
        .description = "Flat-rolled products",
        .validity_start_date = "1972-01-01T00:00:00.000Z",
    };
    int ok = doc != NULL &&
             classification_section_jsonapi_resource(doc, &section) != NULL &&
             classification_chapter_jsonapi_resource(doc, &chapter, NULL, "uk") != NULL &&
             classification_heading_jsonapi_resource(doc, &heading, "uk") != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
