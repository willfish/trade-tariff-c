CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Wno-overlength-strings -pedantic -O2
CPPFLAGS ?= -D_POSIX_C_SOURCE=200809L -Isrc
LDFLAGS ?=
ASAN_CFLAGS := -std=c11 -Wall -Wextra -Werror -Wno-error=cpp -Wno-overlength-strings -pedantic -O1 -g3 -U_FORTIFY_SOURCE -fsanitize=address,undefined -fno-omit-frame-pointer
ASAN_LDFLAGS := -fsanitize=address,undefined
UBSAN_CFLAGS := -std=c11 -Wall -Wextra -Werror -Wno-error=cpp -Wno-overlength-strings -pedantic -O1 -g3 -U_FORTIFY_SOURCE -fsanitize=undefined -fno-omit-frame-pointer
UBSAN_LDFLAGS := -fsanitize=undefined
LIBPQ_PC_AUTO := $(shell pkg-config --exists libpq 2>/dev/null && echo libpq)
LIBPQ_PC_NIX := $(firstword $(wildcard /nix/store/*-postgresql-*-dev/lib/pkgconfig/libpq.pc) $(wildcard /nix/store/*-libpq-*-dev/lib/pkgconfig/libpq.pc))
LIBPQ_PC := $(if $(LIBPQ_PC_NIX),$(LIBPQ_PC_NIX),$(LIBPQ_PC_AUTO))
LIBPQ_PC_DIR := $(dir $(LIBPQ_PC))
LIBPQ_CFLAGS ?= $(shell if [ "$(LIBPQ_PC)" = "libpq" ]; then pkg-config --cflags libpq; elif [ -n "$(LIBPQ_PC)" ]; then PKG_CONFIG_LIBDIR="$(LIBPQ_PC_DIR)" pkg-config --cflags libpq; fi)
LIBPQ_LIBS ?= $(shell if [ "$(LIBPQ_PC)" = "libpq" ]; then pkg-config --libs libpq; elif [ -n "$(LIBPQ_PC)" ]; then PKG_CONFIG_LIBDIR="$(LIBPQ_PC_DIR)" pkg-config --libs libpq; fi)
YYJSON_PREFIX ?= $(shell pkg-config --variable=prefix yyjson 2>/dev/null)
YYJSON_CFLAGS ?= $(if $(YYJSON_PREFIX),-I$(YYJSON_PREFIX)/include)
YYJSON_LIBS ?= $(if $(YYJSON_PREFIX),-L$(YYJSON_PREFIX)/lib -lyyjson)
CMOCKA_CFLAGS ?= $(shell pkg-config --cflags cmocka 2>/dev/null)
CMOCKA_LIBS ?= $(shell pkg-config --libs cmocka 2>/dev/null)

BUILD_DIR := build
OPLOG_PRESENTER_SRC := $(wildcard src/presenters/oplog/*.c)
SRC := src/main.c src/http.c src/router.c src/controllers/commodity_controller.c src/services/commodity_service.c src/models/commodity.c src/models/footnote.c src/models/additional_code_model.c src/models/appendix_5a_model.c src/models/classification_model.c src/models/duty_calculator_metadata_model.c src/models/duty_expression_model.c src/models/goods_nomenclature_description.c src/models/geographical_area_model.c src/models/guide_model.c src/models/import_trade_summary_model.c src/models/measure_model.c src/models/measure_type_model.c src/models/measure_component_model.c src/models/measure_condition_code_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c src/models/measure_excluded_geographical_area_model.c src/models/measure_legal_act_model.c src/models/measurement_unit_model.c src/models/measurement_unit_qualifier_model.c src/models/quota_order_number_model.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c src/domain/commodity_aggregate.c src/domain/goods_nomenclature_tree.c src/domain/jsonapi_include_index.c src/request/commodity_include_plan.c src/loaders/commodity_load_plan.c src/loaders/commodity_loader.c src/loaders/measure_graph_loader.c src/formatters/description_formatter.c src/presenters/additional_code_presenter.c src/presenters/certificate_presenter.c src/presenters/commodity_presenter.c src/presenters/duty_expression_presenter.c src/presenters/duty_expression_description_presenter.c src/presenters/footnote_presenter.c src/presenters/geographical_area_presenter.c src/presenters/goods_nomenclature_description_presenter.c src/presenters/import_trade_summary_presenter.c src/presenters/measure_component_presenter.c src/presenters/measure_condition_code_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_permutation_presenter.c src/presenters/measure_duty_expression_presenter.c src/presenters/measure_legal_act_presenter.c src/presenters/measure_presenter.c src/presenters/measure_type_presenter.c src/presenters/measurement_unit_presenter.c src/presenters/measurement_unit_qualifier_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/preference_code_presenter.c src/presenters/quota_definition_presenter.c src/presenters/regulation_presenter.c src/presenters/oplog_presenters.c $(OPLOG_PRESENTER_SRC) src/repositories/oplog_table_repository.c src/repositories/commodity_repository.c src/repositories/additional_code_repository.c src/repositories/appendix_5a_repository.c src/repositories/classification_repository.c src/repositories/commodity_footnote_repository.c src/repositories/goods_nomenclature_tree_repository.c src/repositories/geographical_area_repository.c src/repositories/guide_repository.c src/repositories/measure_repository.c src/repositories/measure_type_repository.c src/repositories/measure_component_repository.c src/repositories/measure_condition_code_repository.c src/repositories/measure_condition_repository.c src/repositories/measure_condition_component_repository.c src/repositories/measure_excluded_geographical_area_repository.c src/repositories/measure_footnote_repository.c src/repositories/measure_legal_act_repository.c src/repositories/measurement_unit_repository.c src/repositories/measurement_unit_qualifier_repository.c src/repositories/quota_order_number_repository.c src/repositories/quota_definition_repository.c src/repositories/quota_event_repository.c src/repositories/quota_closed_and_transferred_event_repository.c src/renderers/commodity_tree_slice_renderer.c src/renderers/additional_code_jsonapi_renderer.c src/renderers/classification_jsonapi_renderer.c src/renderers/duty_expression_jsonapi_renderer.c src/renderers/footnote_jsonapi_renderer.c src/renderers/geographical_area_jsonapi_renderer.c src/renderers/guide_jsonapi_renderer.c src/renderers/import_trade_summary_jsonapi_renderer.c src/renderers/jsonapi_attribute.c src/renderers/jsonapi_relationship.c src/renderers/measure_jsonapi_renderer.c src/renderers/measure_component_jsonapi_renderer.c src/renderers/measure_condition_code_jsonapi_renderer.c src/renderers/measure_condition_jsonapi_renderer.c src/renderers/measure_condition_component_jsonapi_renderer.c src/renderers/measure_condition_permutation_jsonapi_renderer.c src/renderers/measure_legal_act_jsonapi_renderer.c src/renderers/measure_type_jsonapi_renderer.c src/renderers/measurement_unit_jsonapi_renderer.c src/renderers/measurement_unit_qualifier_jsonapi_renderer.c src/renderers/quota_order_number_jsonapi_renderer.c src/renderers/quota_definition_jsonapi_renderer.c src/renderers/quota_closed_and_transferred_event_jsonapi_renderer.c src/db.c src/json.c src/json_builder.c src/sql_loader.c
SRC += src/presenters/quota_closed_and_transferred_event_presenter.c
SRC += src/presenters/quota_order_number_presenter.c
SRC += src/presenters/guide_presenter.c
SRC += src/presenters/classification_presenter.c
OBJ := $(SRC:src/%.c=$(BUILD_DIR)/%.o)
DEP := $(OBJ:.o=.d)
BIN := $(BUILD_DIR)/trade-tariff-c
CMOCKA_TESTS := $(BUILD_DIR)/tests/test_description_formatter $(BUILD_DIR)/tests/test_json_builder $(BUILD_DIR)/tests/test_jsonapi_include_index $(BUILD_DIR)/tests/test_jsonapi_relationship $(BUILD_DIR)/tests/test_jsonapi_attribute $(BUILD_DIR)/tests/test_preference_code_presenter $(BUILD_DIR)/tests/test_sql_loader $(BUILD_DIR)/tests/test_commodity_request $(BUILD_DIR)/tests/test_commodity_include_plan $(BUILD_DIR)/tests/test_commodity_load_plan $(BUILD_DIR)/tests/test_commodity_aggregate $(BUILD_DIR)/tests/test_commodity_loader $(BUILD_DIR)/tests/test_goods_nomenclature_tree_repository $(BUILD_DIR)/tests/test_measure_repository $(BUILD_DIR)/tests/test_measure_type_repository $(BUILD_DIR)/tests/test_measure_component_repository $(BUILD_DIR)/tests/test_measure_condition_code_repository $(BUILD_DIR)/tests/test_measure_condition_repository $(BUILD_DIR)/tests/test_measure_condition_component_repository $(BUILD_DIR)/tests/test_geographical_area_repository $(BUILD_DIR)/tests/test_measure_excluded_geographical_area_repository $(BUILD_DIR)/tests/test_additional_code_repository $(BUILD_DIR)/tests/test_appendix_5a_repository $(BUILD_DIR)/tests/test_classification_repository $(BUILD_DIR)/tests/test_commodity_footnote_repository $(BUILD_DIR)/tests/test_guide_repository $(BUILD_DIR)/tests/test_measure_footnote_repository $(BUILD_DIR)/tests/test_measure_legal_act_repository $(BUILD_DIR)/tests/test_measurement_unit_repository $(BUILD_DIR)/tests/test_measurement_unit_qualifier_repository $(BUILD_DIR)/tests/test_quota_order_number_repository $(BUILD_DIR)/tests/test_quota_definition_repository $(BUILD_DIR)/tests/test_quota_event_repository $(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_repository $(BUILD_DIR)/tests/test_additional_code_presenter $(BUILD_DIR)/tests/test_commodity_presenter $(BUILD_DIR)/tests/test_goods_nomenclature_description_presenter $(BUILD_DIR)/tests/test_import_trade_summary_presenter $(BUILD_DIR)/tests/test_measure_presenter $(BUILD_DIR)/tests/test_measure_condition_presenter $(BUILD_DIR)/tests/test_measure_condition_permutation_presenter $(BUILD_DIR)/tests/test_measure_legal_act_presenter $(BUILD_DIR)/tests/test_quota_definition_presenter $(BUILD_DIR)/tests/test_duty_expression_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_component_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_condition_code_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_condition_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_condition_component_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_condition_permutation_jsonapi_renderer $(BUILD_DIR)/tests/test_geographical_area_jsonapi_renderer $(BUILD_DIR)/tests/test_additional_code_jsonapi_renderer $(BUILD_DIR)/tests/test_classification_jsonapi_renderer $(BUILD_DIR)/tests/test_footnote_jsonapi_renderer $(BUILD_DIR)/tests/test_guide_jsonapi_renderer $(BUILD_DIR)/tests/test_import_trade_summary_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_legal_act_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_type_jsonapi_renderer $(BUILD_DIR)/tests/test_measurement_unit_jsonapi_renderer $(BUILD_DIR)/tests/test_measurement_unit_qualifier_jsonapi_renderer $(BUILD_DIR)/tests/test_quota_order_number_jsonapi_renderer $(BUILD_DIR)/tests/test_quota_definition_jsonapi_renderer $(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_jsonapi_renderer $(BUILD_DIR)/tests/test_measure_jsonapi_renderer $(BUILD_DIR)/tests/test_commodity_tree_slice_renderer
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_geographical_area_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_guide_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_quota_order_number_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measurement_unit_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measurement_unit_qualifier_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measure_condition_code_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measure_type_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_footnote_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_duty_expression_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measure_component_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measure_condition_component_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_measure_duty_expression_presenter
CMOCKA_TESTS += $(BUILD_DIR)/tests/test_classification_presenter

.PHONY: all clean debug asan ubsan test test-asan unit cmocka parity parity-all parity-one parity-smoke generate-parity-oracles run

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBPQ_LIBS) $(YYJSON_LIBS)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(YYJSON_CFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/tests/test_description_formatter: tests/unit/test_description_formatter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_json_builder: tests/unit/test_json_builder.c src/json_builder.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_jsonapi_include_index: tests/unit/test_jsonapi_include_index.c src/domain/jsonapi_include_index.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_jsonapi_relationship: tests/unit/test_jsonapi_relationship.c src/renderers/jsonapi_relationship.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_jsonapi_attribute: tests/unit/test_jsonapi_attribute.c src/renderers/jsonapi_attribute.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_preference_code_presenter: tests/unit/test_preference_code_presenter.c src/presenters/preference_code_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_sql_loader: tests/unit/test_sql_loader.c src/sql_loader.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_request: tests/unit/test_commodity_request.c src/models/commodity.c src/request/commodity_include_plan.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_include_plan: tests/unit/test_commodity_include_plan.c src/request/commodity_include_plan.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_load_plan: tests/unit/test_commodity_load_plan.c src/loaders/commodity_load_plan.c src/request/commodity_include_plan.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_aggregate: tests/unit/test_commodity_aggregate.c src/domain/commodity_aggregate.c src/domain/goods_nomenclature_tree.c src/models/footnote.c src/models/additional_code_model.c src/models/appendix_5a_model.c src/models/classification_model.c src/models/duty_calculator_metadata_model.c src/models/duty_expression_model.c src/models/guide_model.c src/models/import_trade_summary_model.c src/models/measure_model.c src/models/measure_type_model.c src/models/measure_component_model.c src/models/measurement_unit_model.c src/models/measurement_unit_qualifier_model.c src/models/measure_condition_code_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c src/models/geographical_area_model.c src/models/measure_excluded_geographical_area_model.c src/models/measure_legal_act_model.c src/models/quota_order_number_model.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_loader: tests/unit/test_commodity_loader.c src/loaders/commodity_loader.c src/loaders/commodity_load_plan.c src/loaders/measure_graph_loader.c src/domain/commodity_aggregate.c src/domain/goods_nomenclature_tree.c src/request/commodity_include_plan.c src/repositories/classification_repository.c src/repositories/commodity_footnote_repository.c src/repositories/goods_nomenclature_tree_repository.c src/repositories/guide_repository.c src/repositories/measure_repository.c src/repositories/measure_type_repository.c src/repositories/measure_component_repository.c src/repositories/measurement_unit_repository.c src/repositories/measurement_unit_qualifier_repository.c src/repositories/measure_condition_code_repository.c src/repositories/measure_condition_repository.c src/repositories/appendix_5a_repository.c src/repositories/measure_condition_component_repository.c src/repositories/geographical_area_repository.c src/repositories/measure_excluded_geographical_area_repository.c src/repositories/additional_code_repository.c src/repositories/measure_footnote_repository.c src/repositories/measure_legal_act_repository.c src/repositories/quota_order_number_repository.c src/repositories/quota_definition_repository.c src/repositories/quota_event_repository.c src/repositories/quota_closed_and_transferred_event_repository.c src/models/footnote.c src/models/additional_code_model.c src/models/appendix_5a_model.c src/models/classification_model.c src/models/duty_expression_model.c src/models/geographical_area_model.c src/models/guide_model.c src/models/import_trade_summary_model.c src/models/measure_model.c src/models/measure_type_model.c src/models/measure_component_model.c src/models/measurement_unit_model.c src/models/measurement_unit_qualifier_model.c src/models/measure_condition_code_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c src/models/measure_excluded_geographical_area_model.c src/models/measure_legal_act_model.c src/models/quota_order_number_model.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c src/presenters/import_trade_summary_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_permutation_presenter.c src/presenters/measure_duty_expression_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_goods_nomenclature_tree_repository: tests/unit/test_goods_nomenclature_tree_repository.c src/repositories/goods_nomenclature_tree_repository.c src/domain/goods_nomenclature_tree.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_repository: tests/unit/test_measure_repository.c src/repositories/measure_repository.c src/models/measure_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_type_repository: tests/unit/test_measure_type_repository.c src/repositories/measure_type_repository.c src/models/measure_type_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_component_repository: tests/unit/test_measure_component_repository.c src/repositories/measure_component_repository.c src/models/measure_component_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_code_repository: tests/unit/test_measure_condition_code_repository.c src/repositories/measure_condition_code_repository.c src/models/measure_condition_code_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_repository: tests/unit/test_measure_condition_repository.c src/repositories/measure_condition_repository.c src/models/measure_condition_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_component_repository: tests/unit/test_measure_condition_component_repository.c src/repositories/measure_condition_component_repository.c src/models/measure_condition_component_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_geographical_area_repository: tests/unit/test_geographical_area_repository.c src/repositories/geographical_area_repository.c src/models/geographical_area_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_excluded_geographical_area_repository: tests/unit/test_measure_excluded_geographical_area_repository.c src/repositories/measure_excluded_geographical_area_repository.c src/models/measure_excluded_geographical_area_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_additional_code_repository: tests/unit/test_additional_code_repository.c src/repositories/additional_code_repository.c src/models/additional_code_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_appendix_5a_repository: tests/unit/test_appendix_5a_repository.c src/repositories/appendix_5a_repository.c src/models/appendix_5a_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_classification_repository: tests/unit/test_classification_repository.c src/repositories/classification_repository.c src/models/classification_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_footnote_repository: tests/unit/test_commodity_footnote_repository.c src/repositories/commodity_footnote_repository.c src/models/footnote.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_guide_repository: tests/unit/test_guide_repository.c src/repositories/guide_repository.c src/models/guide_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_footnote_repository: tests/unit/test_measure_footnote_repository.c src/repositories/measure_footnote_repository.c src/models/footnote.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_legal_act_repository: tests/unit/test_measure_legal_act_repository.c src/repositories/measure_legal_act_repository.c src/models/measure_legal_act_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measurement_unit_repository: tests/unit/test_measurement_unit_repository.c src/repositories/measurement_unit_repository.c src/models/measurement_unit_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measurement_unit_qualifier_repository: tests/unit/test_measurement_unit_qualifier_repository.c src/repositories/measurement_unit_qualifier_repository.c src/models/measurement_unit_qualifier_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_order_number_repository: tests/unit/test_quota_order_number_repository.c src/repositories/quota_order_number_repository.c src/models/quota_order_number_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_definition_repository: tests/unit/test_quota_definition_repository.c src/repositories/quota_definition_repository.c src/models/quota_definition_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_event_repository: tests/unit/test_quota_event_repository.c src/repositories/quota_event_repository.c src/models/quota_event_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_repository: tests/unit/test_quota_closed_and_transferred_event_repository.c src/repositories/quota_closed_and_transferred_event_repository.c src/models/quota_closed_and_transferred_event_model.c src/sql_loader.c src/db.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LIBPQ_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(LIBPQ_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_presenter: tests/unit/test_commodity_presenter.c src/presenters/commodity_presenter.c src/presenters/measure_duty_expression_presenter.c src/domain/commodity_aggregate.c src/domain/goods_nomenclature_tree.c src/models/footnote.c src/models/additional_code_model.c src/models/appendix_5a_model.c src/models/classification_model.c src/models/duty_calculator_metadata_model.c src/models/duty_expression_model.c src/models/guide_model.c src/models/import_trade_summary_model.c src/models/measure_model.c src/models/measure_type_model.c src/models/measure_component_model.c src/models/measurement_unit_model.c src/models/measurement_unit_qualifier_model.c src/models/measure_condition_code_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c src/models/geographical_area_model.c src/models/measure_excluded_geographical_area_model.c src/models/measure_legal_act_model.c src/models/quota_order_number_model.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_additional_code_presenter: tests/unit/test_additional_code_presenter.c src/presenters/additional_code_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_goods_nomenclature_description_presenter: tests/unit/test_goods_nomenclature_description_presenter.c src/presenters/goods_nomenclature_description_presenter.c src/models/goods_nomenclature_description.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_import_trade_summary_presenter: tests/unit/test_import_trade_summary_presenter.c src/presenters/import_trade_summary_presenter.c src/presenters/measure_duty_expression_presenter.c src/models/duty_expression_model.c src/models/import_trade_summary_model.c src/models/measure_model.c src/models/measure_component_model.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_duty_expression_presenter: tests/unit/test_measure_duty_expression_presenter.c src/presenters/measure_duty_expression_presenter.c src/models/duty_expression_model.c src/models/measure_model.c src/models/measure_component_model.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_classification_presenter: tests/unit/test_classification_presenter.c src/presenters/classification_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_geographical_area_presenter: tests/unit/test_geographical_area_presenter.c src/presenters/geographical_area_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_guide_presenter: tests/unit/test_guide_presenter.c src/presenters/guide_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_footnote_presenter: tests/unit/test_footnote_presenter.c src/presenters/footnote_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_duty_expression_presenter: tests/unit/test_duty_expression_presenter.c src/presenters/duty_expression_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_presenter: tests/unit/test_measure_presenter.c src/presenters/measure_presenter.c src/models/measure_model.c src/models/measure_component_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_component_presenter: tests/unit/test_measure_component_presenter.c src/presenters/measure_component_presenter.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_definition_presenter: tests/unit/test_quota_definition_presenter.c src/presenters/quota_definition_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_order_number_presenter: tests/unit/test_quota_order_number_presenter.c src/presenters/quota_order_number_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_presenter: tests/unit/test_measure_condition_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/models/appendix_5a_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_component_presenter: tests/unit/test_measure_condition_component_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_code_presenter: tests/unit/test_measure_condition_code_presenter.c src/presenters/measure_condition_code_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_type_presenter: tests/unit/test_measure_type_presenter.c src/presenters/measure_type_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_permutation_presenter: tests/unit/test_measure_condition_permutation_presenter.c src/presenters/measure_condition_permutation_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_legal_act_presenter: tests/unit/test_measure_legal_act_presenter.c src/presenters/measure_legal_act_presenter.c src/models/measure_legal_act_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measurement_unit_presenter: tests/unit/test_measurement_unit_presenter.c src/presenters/measurement_unit_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measurement_unit_qualifier_presenter: tests/unit/test_measurement_unit_qualifier_presenter.c src/presenters/measurement_unit_qualifier_presenter.c src/presenters/goods_nomenclature_description_presenter.c src/models/goods_nomenclature_description.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_presenter: tests/unit/test_quota_closed_and_transferred_event_presenter.c src/presenters/quota_closed_and_transferred_event_presenter.c src/models/quota_closed_and_transferred_event_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_duty_expression_jsonapi_renderer: tests/unit/test_duty_expression_jsonapi_renderer.c src/renderers/duty_expression_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/presenters/duty_expression_presenter.c src/presenters/measure_duty_expression_presenter.c src/models/duty_expression_model.c src/models/measure_component_model.c src/models/measure_model.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_type_jsonapi_renderer: tests/unit/test_measure_type_jsonapi_renderer.c src/renderers/measure_type_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measure_type_model.c src/presenters/measure_type_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_component_jsonapi_renderer: tests/unit/test_measure_component_jsonapi_renderer.c src/renderers/measure_component_jsonapi_renderer.c src/renderers/jsonapi_attribute.c src/renderers/jsonapi_relationship.c src/models/goods_nomenclature_description.c src/models/measure_component_model.c src/presenters/goods_nomenclature_description_presenter.c src/presenters/measure_component_presenter.c src/presenters/measurement_unit_presenter.c src/presenters/measurement_unit_qualifier_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_code_jsonapi_renderer: tests/unit/test_measure_condition_code_jsonapi_renderer.c src/renderers/measure_condition_code_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measure_condition_code_model.c src/presenters/measure_condition_code_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_jsonapi_renderer: tests/unit/test_measure_condition_jsonapi_renderer.c src/renderers/measure_condition_jsonapi_renderer.c src/renderers/jsonapi_attribute.c src/renderers/jsonapi_relationship.c src/renderers/measure_condition_code_jsonapi_renderer.c src/renderers/measure_condition_component_jsonapi_renderer.c src/models/appendix_5a_model.c src/models/measure_condition_model.c src/models/measure_condition_code_model.c src/models/measure_condition_component_model.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_code_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_component_jsonapi_renderer: tests/unit/test_measure_condition_component_jsonapi_renderer.c src/renderers/measure_condition_component_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measure_condition_component_model.c src/presenters/measure_condition_component_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_condition_permutation_jsonapi_renderer: tests/unit/test_measure_condition_permutation_jsonapi_renderer.c src/renderers/measure_condition_permutation_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measure_condition_permutation_model.c src/presenters/measure_condition_permutation_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_geographical_area_jsonapi_renderer: tests/unit/test_geographical_area_jsonapi_renderer.c src/renderers/geographical_area_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/geographical_area_model.c src/models/measure_excluded_geographical_area_model.c src/presenters/geographical_area_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_additional_code_jsonapi_renderer: tests/unit/test_additional_code_jsonapi_renderer.c src/renderers/additional_code_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/additional_code_model.c src/presenters/additional_code_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_classification_jsonapi_renderer: tests/unit/test_classification_jsonapi_renderer.c src/renderers/classification_jsonapi_renderer.c src/renderers/guide_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/classification_model.c src/models/goods_nomenclature_description.c src/models/guide_model.c src/presenters/classification_presenter.c src/presenters/goods_nomenclature_description_presenter.c src/presenters/guide_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_footnote_jsonapi_renderer: tests/unit/test_footnote_jsonapi_renderer.c src/renderers/footnote_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/footnote.c src/presenters/footnote_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_guide_jsonapi_renderer: tests/unit/test_guide_jsonapi_renderer.c src/renderers/guide_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/guide_model.c src/presenters/guide_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_import_trade_summary_jsonapi_renderer: tests/unit/test_import_trade_summary_jsonapi_renderer.c src/renderers/import_trade_summary_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/import_trade_summary_model.c src/presenters/import_trade_summary_presenter.c src/presenters/measure_duty_expression_presenter.c src/models/measure_model.c src/models/measure_component_model.c src/models/duty_expression_model.c src/presenters/duty_expression_description_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_legal_act_jsonapi_renderer: tests/unit/test_measure_legal_act_jsonapi_renderer.c src/renderers/measure_legal_act_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measure_legal_act_model.c src/presenters/measure_legal_act_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measurement_unit_jsonapi_renderer: tests/unit/test_measurement_unit_jsonapi_renderer.c src/renderers/measurement_unit_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measurement_unit_model.c src/presenters/measurement_unit_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measurement_unit_qualifier_jsonapi_renderer: tests/unit/test_measurement_unit_qualifier_jsonapi_renderer.c src/renderers/measurement_unit_qualifier_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/measurement_unit_qualifier_model.c src/presenters/measurement_unit_qualifier_presenter.c src/models/goods_nomenclature_description.c src/presenters/goods_nomenclature_description_presenter.c src/formatters/description_formatter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_order_number_jsonapi_renderer: tests/unit/test_quota_order_number_jsonapi_renderer.c src/renderers/quota_order_number_jsonapi_renderer.c src/renderers/quota_definition_jsonapi_renderer.c src/renderers/quota_closed_and_transferred_event_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/quota_order_number_model.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c src/presenters/quota_definition_presenter.c
$(BUILD_DIR)/tests/test_quota_order_number_jsonapi_renderer: src/presenters/quota_closed_and_transferred_event_presenter.c
$(BUILD_DIR)/tests/test_quota_order_number_jsonapi_renderer: src/presenters/quota_order_number_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_definition_jsonapi_renderer: tests/unit/test_quota_definition_jsonapi_renderer.c src/renderers/quota_definition_jsonapi_renderer.c src/renderers/quota_closed_and_transferred_event_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c src/presenters/quota_definition_presenter.c
$(BUILD_DIR)/tests/test_quota_definition_jsonapi_renderer: src/presenters/quota_closed_and_transferred_event_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_jsonapi_renderer: tests/unit/test_quota_closed_and_transferred_event_jsonapi_renderer.c src/renderers/quota_closed_and_transferred_event_jsonapi_renderer.c src/renderers/jsonapi_relationship.c src/models/quota_closed_and_transferred_event_model.c
$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_jsonapi_renderer: src/presenters/quota_closed_and_transferred_event_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: tests/unit/test_measure_jsonapi_renderer.c src/renderers/measure_jsonapi_renderer.c src/renderers/footnote_jsonapi_renderer.c src/renderers/geographical_area_jsonapi_renderer.c src/renderers/jsonapi_attribute.c src/renderers/jsonapi_relationship.c src/renderers/measure_component_jsonapi_renderer.c src/renderers/measure_condition_code_jsonapi_renderer.c src/renderers/measure_condition_jsonapi_renderer.c src/renderers/measure_condition_component_jsonapi_renderer.c src/renderers/measure_condition_permutation_jsonapi_renderer.c src/renderers/measure_legal_act_jsonapi_renderer.c src/models/footnote.c src/models/appendix_5a_model.c src/models/duty_expression_model.c src/models/measure_model.c src/models/geographical_area_model.c src/models/measure_excluded_geographical_area_model.c src/models/measure_component_model.c src/models/measure_condition_code_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c src/models/measure_legal_act_model.c src/presenters/footnote_presenter.c src/presenters/preference_code_presenter.c src/presenters/measure_presenter.c src/presenters/geographical_area_presenter.c src/presenters/measure_component_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_code_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/measure_duty_expression_presenter.c src/presenters/measure_legal_act_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/formatters/description_formatter.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/presenters/additional_code_presenter.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/presenters/duty_expression_presenter.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/presenters/measure_condition_permutation_presenter.c src/presenters/measure_type_presenter.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/models/goods_nomenclature_description.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/presenters/goods_nomenclature_description_presenter.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/presenters/measurement_unit_presenter.c src/presenters/measurement_unit_qualifier_presenter.c
$(BUILD_DIR)/tests/test_measure_jsonapi_renderer: src/presenters/quota_order_number_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: tests/unit/test_commodity_tree_slice_renderer.c src/renderers/commodity_tree_slice_renderer.c src/request/commodity_include_plan.c src/renderers/additional_code_jsonapi_renderer.c src/renderers/classification_jsonapi_renderer.c src/renderers/duty_expression_jsonapi_renderer.c src/renderers/footnote_jsonapi_renderer.c src/renderers/geographical_area_jsonapi_renderer.c src/renderers/guide_jsonapi_renderer.c src/renderers/import_trade_summary_jsonapi_renderer.c src/renderers/jsonapi_attribute.c src/renderers/jsonapi_relationship.c src/renderers/measure_jsonapi_renderer.c src/renderers/measure_component_jsonapi_renderer.c src/renderers/measure_condition_code_jsonapi_renderer.c src/renderers/measure_condition_jsonapi_renderer.c src/renderers/measure_condition_component_jsonapi_renderer.c src/renderers/measure_condition_permutation_jsonapi_renderer.c src/renderers/measure_legal_act_jsonapi_renderer.c src/renderers/measure_type_jsonapi_renderer.c src/renderers/measurement_unit_jsonapi_renderer.c src/renderers/measurement_unit_qualifier_jsonapi_renderer.c src/renderers/quota_order_number_jsonapi_renderer.c src/renderers/quota_definition_jsonapi_renderer.c src/renderers/quota_closed_and_transferred_event_jsonapi_renderer.c src/domain/commodity_aggregate.c src/domain/goods_nomenclature_tree.c src/domain/jsonapi_include_index.c src/models/footnote.c src/models/additional_code_model.c src/models/appendix_5a_model.c src/models/classification_model.c src/models/duty_calculator_metadata_model.c src/models/duty_expression_model.c src/models/guide_model.c src/models/import_trade_summary_model.c src/models/measure_model.c src/models/measure_type_model.c src/models/geographical_area_model.c src/models/measure_excluded_geographical_area_model.c src/models/measure_component_model.c src/models/measurement_unit_model.c src/models/measurement_unit_qualifier_model.c src/models/measure_condition_code_model.c src/models/measure_condition_model.c src/models/measure_condition_component_model.c src/models/measure_condition_permutation_model.c src/models/measure_legal_act_model.c src/models/quota_order_number_model.c src/models/quota_definition_model.c src/models/quota_event_model.c src/models/quota_closed_and_transferred_event_model.c src/models/goods_nomenclature_description.c src/presenters/additional_code_presenter.c src/presenters/commodity_presenter.c src/presenters/duty_expression_presenter.c src/presenters/footnote_presenter.c src/presenters/goods_nomenclature_description_presenter.c src/presenters/guide_presenter.c src/presenters/preference_code_presenter.c src/presenters/measure_presenter.c src/presenters/geographical_area_presenter.c src/presenters/measure_component_presenter.c src/presenters/measure_condition_presenter.c src/presenters/measure_condition_component_presenter.c src/presenters/measure_duty_expression_presenter.c src/presenters/measure_legal_act_presenter.c src/presenters/monetary_unit_presenter.c src/presenters/duty_expression_description_presenter.c src/presenters/quota_definition_presenter.c src/formatters/description_formatter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/quota_closed_and_transferred_event_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/quota_order_number_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/measurement_unit_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/measurement_unit_qualifier_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/measure_condition_code_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/measure_condition_permutation_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/measure_type_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/import_trade_summary_presenter.c
$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer: src/presenters/classification_presenter.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(YYJSON_CFLAGS) $(CMOCKA_CFLAGS) $(CFLAGS) $^ -o $@ $(YYJSON_LIBS) $(CMOCKA_LIBS)

debug:
	$(MAKE) CFLAGS='-std=c11 -Wall -Wextra -Werror -Wno-error=cpp -Wno-overlength-strings -pedantic -O0 -g3 -U_FORTIFY_SOURCE'

asan:
	$(MAKE) CFLAGS='$(ASAN_CFLAGS)' LDFLAGS='$(ASAN_LDFLAGS)'

ubsan:
	$(MAKE) CFLAGS='$(UBSAN_CFLAGS)' LDFLAGS='$(UBSAN_LDFLAGS)'

run: $(BIN)
	$(BIN)

unit: $(BIN) cmocka
	@SERVICE=uk PORT=0 $(BIN) --self-test

cmocka: $(CMOCKA_TESTS)
	@$(BUILD_DIR)/tests/test_description_formatter
	@$(BUILD_DIR)/tests/test_json_builder
	@$(BUILD_DIR)/tests/test_jsonapi_include_index
	@$(BUILD_DIR)/tests/test_jsonapi_relationship
	@$(BUILD_DIR)/tests/test_jsonapi_attribute
	@$(BUILD_DIR)/tests/test_preference_code_presenter
	@$(BUILD_DIR)/tests/test_sql_loader
	@$(BUILD_DIR)/tests/test_commodity_request
	@$(BUILD_DIR)/tests/test_commodity_include_plan
	@$(BUILD_DIR)/tests/test_commodity_load_plan
	@$(BUILD_DIR)/tests/test_commodity_aggregate
	@$(BUILD_DIR)/tests/test_commodity_loader
	@$(BUILD_DIR)/tests/test_goods_nomenclature_tree_repository
	@$(BUILD_DIR)/tests/test_measure_repository
	@$(BUILD_DIR)/tests/test_measure_type_repository
	@$(BUILD_DIR)/tests/test_measure_component_repository
	@$(BUILD_DIR)/tests/test_measure_condition_code_repository
	@$(BUILD_DIR)/tests/test_measure_condition_repository
	@$(BUILD_DIR)/tests/test_measure_condition_component_repository
	@$(BUILD_DIR)/tests/test_geographical_area_repository
	@$(BUILD_DIR)/tests/test_measure_excluded_geographical_area_repository
	@$(BUILD_DIR)/tests/test_additional_code_repository
	@$(BUILD_DIR)/tests/test_appendix_5a_repository
	@$(BUILD_DIR)/tests/test_classification_repository
	@$(BUILD_DIR)/tests/test_commodity_footnote_repository
	@$(BUILD_DIR)/tests/test_guide_repository
	@$(BUILD_DIR)/tests/test_measure_footnote_repository
	@$(BUILD_DIR)/tests/test_measure_legal_act_repository
	@$(BUILD_DIR)/tests/test_measurement_unit_repository
	@$(BUILD_DIR)/tests/test_measurement_unit_qualifier_repository
	@$(BUILD_DIR)/tests/test_quota_order_number_repository
	@$(BUILD_DIR)/tests/test_quota_definition_repository
	@$(BUILD_DIR)/tests/test_quota_event_repository
	@$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_repository
	@$(BUILD_DIR)/tests/test_commodity_presenter
	@$(BUILD_DIR)/tests/test_goods_nomenclature_description_presenter
	@$(BUILD_DIR)/tests/test_import_trade_summary_presenter
	@$(BUILD_DIR)/tests/test_geographical_area_presenter
	@$(BUILD_DIR)/tests/test_guide_presenter
	@$(BUILD_DIR)/tests/test_footnote_presenter
	@$(BUILD_DIR)/tests/test_duty_expression_presenter
	@$(BUILD_DIR)/tests/test_measure_component_presenter
	@$(BUILD_DIR)/tests/test_measure_condition_component_presenter
	@$(BUILD_DIR)/tests/test_measure_duty_expression_presenter
	@$(BUILD_DIR)/tests/test_classification_presenter
	@$(BUILD_DIR)/tests/test_quota_definition_presenter
	@$(BUILD_DIR)/tests/test_quota_order_number_presenter
	@$(BUILD_DIR)/tests/test_measure_condition_presenter
	@$(BUILD_DIR)/tests/test_measure_condition_code_presenter
	@$(BUILD_DIR)/tests/test_measure_type_presenter
	@$(BUILD_DIR)/tests/test_measure_condition_permutation_presenter
	@$(BUILD_DIR)/tests/test_measure_legal_act_presenter
	@$(BUILD_DIR)/tests/test_measurement_unit_presenter
	@$(BUILD_DIR)/tests/test_measurement_unit_qualifier_presenter
	@$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_presenter
	@$(BUILD_DIR)/tests/test_duty_expression_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_component_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_condition_code_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_condition_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_condition_component_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_condition_permutation_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_geographical_area_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_additional_code_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_classification_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_footnote_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_guide_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_import_trade_summary_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_legal_act_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_type_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measurement_unit_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measurement_unit_qualifier_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_quota_order_number_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_quota_definition_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_quota_closed_and_transferred_event_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_measure_jsonapi_renderer
	@$(BUILD_DIR)/tests/test_commodity_tree_slice_renderer

test: unit
	@PARITY_SMOKE_REQUIRED=0 ./scripts/parity_smoke.sh

test-asan:
	$(MAKE) clean
	$(MAKE) CFLAGS='$(ASAN_CFLAGS)' LDFLAGS='$(ASAN_LDFLAGS)' test

parity: $(BIN)
	./scripts/parity_commodity.sh

parity-all: $(BIN)
	./scripts/parity_all_commodities.sh

parity-one: $(BIN)
	./scripts/parity_one_commodity.sh

parity-smoke: $(BIN)
	./scripts/parity_smoke.sh

generate-parity-oracles: $(BIN)
	./scripts/generate_parity_oracles.sh

clean:
	rm -rf $(BUILD_DIR)

-include $(DEP)
