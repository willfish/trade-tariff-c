#ifndef TRADE_TARIFF_C_DUTY_CALCULATOR_METADATA_MODEL_H
#define TRADE_TARIFF_C_DUTY_CALCULATOR_METADATA_MODEL_H

#include <stddef.h>

typedef struct {
    char *key;
    char *description;
} DutyCalculatorVatOptionModel;

typedef struct {
    char *code;
    char *overlay;
    char *hint;
    char *geographical_area_id;
    char *measure_sid;
} DutyCalculatorAdditionalCodeAnnotationModel;

typedef struct {
    DutyCalculatorAdditionalCodeAnnotationModel *items;
    size_t len;
    size_t cap;
} DutyCalculatorAdditionalCodeAnnotationModelList;

typedef struct {
    char *measure_type_id;
    char *measure_type_description;
    char *heading_overlay;
    char *heading_hint;
    DutyCalculatorAdditionalCodeAnnotationModelList additional_codes;
} DutyCalculatorAdditionalCodeGroupModel;

typedef struct {
    DutyCalculatorAdditionalCodeGroupModel *items;
    size_t len;
    size_t cap;
} DutyCalculatorAdditionalCodeGroupModelList;

typedef struct {
    char *value;
    int present;
    int is_null;
} DutyCalculatorMetadataField;

typedef struct {
    char *key;
    DutyCalculatorMetadataField measurement_unit_code;
    DutyCalculatorMetadataField measurement_unit_qualifier_code;
    DutyCalculatorMetadataField abbreviation;
    DutyCalculatorMetadataField expansion;
    DutyCalculatorMetadataField unit_question;
    DutyCalculatorMetadataField unit_hint;
    DutyCalculatorMetadataField unit;
    DutyCalculatorMetadataField multiplier;
    DutyCalculatorMetadataField coerced_measurement_unit_code;
    DutyCalculatorMetadataField original_unit;
    DutyCalculatorMetadataField measurement_unit_type;
} DutyCalculatorMeasureUnitModel;

typedef struct {
    DutyCalculatorVatOptionModel *items;
    size_t len;
    size_t cap;
} DutyCalculatorVatOptionModelList;

typedef struct {
    DutyCalculatorMeasureUnitModel *items;
    size_t len;
    size_t cap;
} DutyCalculatorMeasureUnitModelList;

void duty_calculator_additional_code_annotation_model_free(DutyCalculatorAdditionalCodeAnnotationModel *model);
void duty_calculator_additional_code_annotation_model_list_free(DutyCalculatorAdditionalCodeAnnotationModelList *list);
int duty_calculator_additional_code_annotation_model_list_append(DutyCalculatorAdditionalCodeAnnotationModelList *list,
                                                                 DutyCalculatorAdditionalCodeAnnotationModel model);
void duty_calculator_additional_code_group_model_free(DutyCalculatorAdditionalCodeGroupModel *model);
void duty_calculator_additional_code_group_model_list_free(DutyCalculatorAdditionalCodeGroupModelList *list);
int duty_calculator_additional_code_group_model_list_append(DutyCalculatorAdditionalCodeGroupModelList *list,
                                                            DutyCalculatorAdditionalCodeGroupModel model);
void duty_calculator_metadata_field_free(DutyCalculatorMetadataField *field);
void duty_calculator_vat_option_model_free(DutyCalculatorVatOptionModel *model);
void duty_calculator_vat_option_model_list_free(DutyCalculatorVatOptionModelList *list);
int duty_calculator_vat_option_model_list_append(DutyCalculatorVatOptionModelList *list,
                                                 DutyCalculatorVatOptionModel model);
void duty_calculator_measure_unit_model_free(DutyCalculatorMeasureUnitModel *model);
void duty_calculator_measure_unit_model_list_free(DutyCalculatorMeasureUnitModelList *list);
int duty_calculator_measure_unit_model_list_append(DutyCalculatorMeasureUnitModelList *list,
                                                   DutyCalculatorMeasureUnitModel model);
int duty_calculator_metadata_model_self_test(void);

#endif
