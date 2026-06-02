# C Implementation Q&A

This is a review pack for navigating the C commodity endpoint. It is written as questions to ask while reading the code, with the files that answer each question.

If you already know the Rails trade-tariff domain and want the C-specific patterns instead, read `docs/c-idioms.md` first.

## Quick Map

Read these files in this order for the main path:

1. `src/main.c` - process entry point and `--self-test`.
2. `src/http.c` - blocking HTTP accept/read/write loop.
3. `src/router.c` - route recognition and service selection.
4. `src/controllers/commodity_controller.c` - request validation.
5. `src/services/commodity_service.c` - service facade and self-test composition.
6. `src/loaders/commodity_loader.c` - include-plan-aware typed aggregate loading.
7. `src/loaders/measure_graph_loader.c` - measure relationship graph loading.
8. `src/domain/commodity_aggregate.c` - owned response aggregate and derived keys.
9. `src/renderers/commodity_tree_slice_renderer.c` - yyjson JSON:API document rendering.
10. `scripts/parity_one_commodity.rb` and `scripts/parity_smoke.sh` - Rails parity checks.

The short version: HTTP routing is small; the commodity response path now loads typed rows into an aggregate and renders JSON:API with yyjson.

## Contract

### What does this C service currently try to replicate?

It targets one Rails vertical: unfiltered commodity show responses for:

- `GET /api/commodities/:id`
- `GET /uk/api/commodities/:id`
- `GET /xi/api/commodities/:id`

The Rails path being mirrored is:

`Api::V2::CommoditiesController#find_commodity -> CachedCommodityService -> CommodityPresenter -> Api::V2::Commodities::CommoditySerializer -> optional ResponseFilter`.

The corresponding Rails files are:

- `../app/controllers/api/v2/commodities_controller.rb`
- `../app/services/cached_commodity_service.rb`
- `../app/presenters/api/v2/commodities/commodity_presenter.rb`
- `../app/serializers/api/v2/commodities/commodity_serializer.rb`

### Are Rails filters supported?

Only a narrow subset is supported. `include=` is parsed against the static include registry in `src/request/commodity_include_plan.c`, and unsupported include paths return a structured JSON:API error. `filter[meursing_additional_code_id]` is accepted and passed into the typed request context. Other query parameters are rejected in `src/controllers/commodity_controller.c`.

### How does service selection work?

`src/router.c` selects `uk` or `xi` from the path prefix. For `/api/commodities/:id`, it falls back to the `SERVICE` environment variable and defaults to `uk`.

The commodity request validator in `src/models/commodity.c` only accepts `uk` and `xi`.

## Request Flow

### What happens when a request arrives?

`src/http.c` accepts a connection, reads a single request buffer, parses method and path with `sscanf`, rejects query strings, routes the request, writes a JSON:API response, and closes the connection.

This is a minimal blocking HTTP server. It is enough for parity work, but it is not production-grade HTTP infrastructure.

### Where are request parameters validated?

`src/controllers/commodity_controller.c` builds a `CommodityRequest` and calls `commodity_request_validate`.

Validation checks:

- service is `uk` or `xi`
- commodity id is exactly ten digits
- `ACTUAL_DATE`, when present, is shaped like `YYYY-MM-DD`

The date check is syntactic, not calendar-aware.

### Where does `ACTUAL_DATE` affect behaviour?

The controller validates it, then repositories receive it through `CommodityRequestContext` and build date predicates from that request-scoped value. If absent, SQL uses `CURRENT_DATE`.

Important files:

- `src/controllers/commodity_controller.c`
- `src/loaders/commodity_loader.c`
- `src/repositories/*_repository.c`

## Data Access

### How does C talk to Postgres?

`src/db.c` uses `libpq`. It keeps one process-global connection and one process-global prepared statement cache.

Main functions:

- `db_fetch_single_text` - retained for simple text queries.
- `db_fetch_rows` - used for row-shaped repository queries.

This is simple, but not concurrency-safe if the HTTP server becomes multi-threaded.

### Does SQL use bound parameters?

Commodity id is passed as `$1` through prepared statements.

The schema name and date expression are interpolated into SQL strings. The schema is constrained by service validation (`uk` or `xi`). `ACTUAL_DATE` is only shape-validated before interpolation, so keep that validator strict if expanding date support.

### What is the repository layer doing?

`src/repositories/oplog_table_repository.c` is a generic helper for fetching table rows from schema-qualified oplog tables. It validates identifier-shaped schema/table names.

The commodity endpoint uses row-shaped repositories for the root commodity/tree slice, classification, guides, footnotes, measures, measure components, conditions, geographical areas, legal acts, measurement units, additional codes, and quotas. The loader decides which repository families to call from `CommodityLoadPlan`.

## Rendering

### Where is the main JSON response built?

`src/renderers/commodity_tree_slice_renderer.c`.

The renderer receives a fully-loaded `CommodityAggregate` and a `CommodityIncludePlan`, then builds the JSON:API document with yyjson. It should not query the database.

### What major resources does the typed renderer build?

At a high level:

- root commodity resource
- section, chapter, heading, ancestors
- commodity and heading footnotes
- applicable measures inherited from commodity and ancestors
- measure components, conditions, condition permutations
- geographical areas and excluded countries
- legal acts and regulations
- quota order numbers and definitions
- preference codes
- import trade summary
- duty calculator metadata placeholders

Included resource de-duplication is handled by `src/domain/jsonapi_include_index.c`. Relationship linkage is rendered independently from inclusion where the typed renderer has the data.

### What does yyjson do here?

`yyjson` builds the whole commodity response document in the renderer. Per-resource renderers return `yyjson_mut_val *` resources, and the top-level renderer writes the final owned JSON string.

### Is the whole response now built with a JSON DOM?

Yes for the commodity response. The root document and included resources are created through yyjson renderer functions. `src/json_builder.c` remains a small helper used by tests/self-tests, not the main commodity path.

### What is still string-based?

The commodity response path should not rely on response string rewriting. Formatting belongs in named presenters and renderers, for example description formatting, duty-expression presentation, preference-code presentation, and measurement-unit presentation.

## Rails Parity

### What does parity mean?

Parity means the C response and Rails response are byte-for-byte equivalent after stable JSON key sorting and included-resource sorting.

The comparison logic lives in `scripts/parity_one_commodity.rb`.

### Where does the Rails oracle come from?

The parity scripts either:

- run under `rails runner` and call `CachedCommodityService`, or
- read an existing oracle snapshot from `build/parity-oracle/<service>/<date>/<commodity_id>.json`.

### What does `make test` prove?

`make test` runs:

1. the compiled-in C self-tests through `--self-test`
2. cmocka unit tests under `tests/unit`
3. parity smoke in optional read mode (`PARITY_SMOKE_REQUIRED=0`)

If local oracle snapshots exist, smoke parity checks them. If no snapshots exist, the smoke step can skip during `make test`.

### What does `make parity-smoke` prove?

`make parity-smoke` requires the smoke fixture file and matching oracle snapshots. It starts the C server, compares every listed commodity in `tests/parity-smoke/<service>.txt`, and fails on missing or mismatched snapshots.

### What does `make parity-all` prove?

It runs a broader commodity set through Rails-oracle comparison. This is the expensive completion gate for endpoint parity.

## Tests And Tooling

### What test layers exist?

- Built-in self-tests called by `--self-test`.
- cmocka unit tests under `tests/unit`.
- smoke parity cases under `tests/parity-smoke`.
- one-commodity and all-commodity parity scripts under `scripts/`.

### What sanitizer support exists?

The Makefile has:

- `make asan`
- `make ubsan`
- `make test-asan`

These are important for C string and ownership work. Use them before trusting refactors in typed models, aggregate ownership, repository hydration, renderer construction, or manual allocation code.

### What should be added when a parity failure is fixed?

Add the commodity id and short reason to `tests/parity-smoke/<service>.txt`, then refresh or create the corresponding oracle snapshot.

Use a reason that explains the class of behaviour, not just the code, for example:

`7312108112 applicable-additional-codes-single-none-option`

## Current Risks

### What is the biggest architectural risk?

The main architectural risk is incomplete parity across the typed loader/presenter boundary. The remaining work is to keep moving behaviour into named model, repository, domain, presenter, and renderer modules while proving parity against Rails.

### What is the biggest correctness risk?

Parity-sensitive formatting can drift from Rails if it is hidden in broad renderer code. Prefer small presenters with unit tests for isolated rules, plus parity smoke cases for endpoint-level behaviour.

### What is the biggest operational risk?

The HTTP and DB layers use blocking, process-global state. That is fine for a parity spike, but production use would need an explicit runtime decision: single-process behind a supervisor, multi-process, event loop, or threaded server with per-worker DB state.

## Review Checklist

Use this checklist when deep-diving a behaviour:

1. Find the Rails source of truth first.
2. Find the C implementation site in repository, domain, presenter, or renderer code.
3. Run `SERVICE=uk COMMODITY_ID=<id> make parity-one`.
4. If it fails, inspect `build/parity-failures/<service>/<id>/rails.json` and `c.json`.
5. Fix the smallest named behaviour, not the first string difference.
6. Add or update a cmocka unit test when the behaviour can be isolated.
7. Add the commodity to `tests/parity-smoke/<service>.txt` when the issue is endpoint-level.
8. Run `make test` and, for risky memory/string changes, `make test-asan`.

## Questions To Ask While Reading

### Request and contract

- Why does this path resolve to `uk` or `xi`?
- Would Rails accept this request, or should C reject it?
- Is this an unfiltered request? If not, is it intentionally out of scope?

### Commodity lookup

- Does the C lookup match `Commodity.actual.non_hidden.declarable.by_code`?
- Is the commodity a leaf by the same nested-set semantics as Rails?
- Is `ACTUAL_DATE` being applied consistently to goods nomenclature, tree node, description, indent, and measure records?

### Measures

- Are measures loaded from the commodity and every relevant ancestor?
- Does de-duplication match Rails `Measure.dedupe_similar`?
- Are excluded measure types handled in the same place as Rails `without_excluded_types`?
- Are import/export flags based on Rails-equivalent `trade_movement_code` rules?

### Metadata

- Is the duty calculator field computed from loaded typed rows in `commodity_presenter.c`?
- Does this field change after country filtering in Rails?
- If Rails delegates to a named service, is there a named C equivalent?

### Rendering

- Is this behaviour in a named presenter or renderer?
- Does the renderer receive already-loaded typed data?
- Could any remaining broad renderer logic be moved into a focused presenter?

### Tests

- Is there a unit test for the isolated rule?
- Is there a parity smoke commodity for the endpoint-level rule?
- Did the fix pass under normal and sanitizer builds?

## Good First Deep-Dive Routes

Start with these topics because they cut across the architecture without requiring the whole tariff domain at once:

1. Query rejection: `src/http.c`, `src/router.c`, `src/models/commodity.c`.
2. Duty calculator metadata: `src/presenters/commodity_presenter.c`, `tests/unit/test_commodity_presenter.c`.
3. Preference code calculation: `src/presenters/preference_code_presenter.c`, `tests/unit/test_preference_code_presenter.c`.
4. Description formatting: `src/formatters/description_formatter.c`, `src/presenters/goods_nomenclature_description_presenter.c`, `tests/unit/test_description_formatter.c`.
5. Parity smoke flow: `scripts/parity_smoke.sh`, `scripts/parity_one_commodity.rb`, `tests/parity-smoke/uk.txt`.
