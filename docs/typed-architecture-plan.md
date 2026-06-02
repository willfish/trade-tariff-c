# Typed Commodity Architecture Plan

This is a handoff plan for migrating the C commodity endpoint away from the current hand-built SQL JSON response and toward a typed, flexible, JSON:API-aware implementation.

The target reader is an agent or engineer implementing the C version. Assume they know C but may not know the Rails Trade Tariff domain.

For day-to-day navigation of the current code, prefer `docs/implementation-qa.md`.

## Current status (2026-06)

The typed path is **production code**. `commodity_service.c` loads a `CommodityAggregate` and renders with `commodity_tree_slice_renderer.c` (yyjson). The archived SQL renderer lives at `sql/legacy/commodity_response_legacy.sql` and is not linked into the binary.

| Phase | State |
|-------|--------|
| 0 Parity gate | `make test-asan` in CI; smoke needs local oracle snapshots |
| 1 SQL files + loader | Done (`sql_loader.c`, per-family `sql/*.sql`) |
| 2 Include plan | Done (`commodity_include_plan.c`, query `include=` supported) |
| 3 Commodity + ancestors | Done |
| 4 Measures | Done (`measure_graph_loader.c`) |
| 5 Measure components | Done |
| 6 Conditions, footnotes, areas, codes | Done (presenter/renderer split) |
| 7 Quotas | Done |
| 8 Remove legacy renderer | Runtime removed; SQL archive kept for reference |

**Remaining work:** exhaustive Rails parity (`make parity-all`), grow `tests/parity-smoke/*.txt`, commit oracle snapshots after `make generate-parity-oracles` with a populated DB, and shrink presenter logic still buried in `commodity_tree_slice_renderer.c`.

Duty-calculator overlay JSON is vendored under `db/` (from `trade-tariff-backend/db/`).

## Goal

Build a C commodity endpoint that can match Rails parity while remaining understandable and extensible.

The long-term shape should be:

```text
HTTP request
  -> route/service/date/include parsing
  -> include plan
  -> batched repository queries over stable read models
  -> typed C aggregate
  -> domain/presenter formatting
  -> yyjson JSON:API document
  -> parity comparison against Rails
```

The current shape is closer to:

```text
HTTP request
  -> one large SQL string
  -> PostgreSQL JSON construction
  -> yyjson patches
  -> string postprocessor
  -> HTTP response
```

The migration should be incremental. Keep the existing SQL response as a parity oracle while replacing one response slice at a time.

## Principles

### Do not mirror every oplog table in C

The `uk` and `xi` schemas contain many `_oplog` tables, plus views and materialized views. The C app should not become a hand-maintained clone of the raw tariff storage model.

Use stable read models:

- materialized views such as `goods_nomenclature_tree_nodes`
- current-facing views such as `measures`, `goods_nomenclatures`, `measure_components`
- small endpoint-specific SQL queries where a view does not already express the needed read model

Raw `_oplog` table structs should be isolated under generated or experimental code unless they are wired into a real repository.

### Presenters must not query

Rendering code should never fetch data.

Bad shape:

```text
render measure
  -> fetch components
  -> render component
  -> fetch measurement unit
```

Good shape:

```text
include plan
  -> fetch measures in one batch
  -> fetch components in one batch
  -> fetch measurement units in one batch
  -> render already-loaded data
```

This is the C equivalent of Rails eager loading.

### Make dynamic includes explicit

Sequel lets associations carry dynamic dataset blocks. C should not try to recreate that full ORM. Instead, define a static include registry and concrete loaders.

Dynamic behaviour should come from selecting include specs, not from runtime reflection.

### Treat nested set as a special domain loader

The Rails nested-set implementation is more than a normal association. Loading ancestors or descendants also populates other relationships in memory.

Do not force that into a generic association abstraction. Give it its own repository and assembler.

## Target Modules

The proposed module layout is:

```text
src/request/
  commodity_request.h
  commodity_include_plan.h

src/repositories/
  commodity_repository.h
  goods_nomenclature_tree_repository.h
  measure_repository.h
  geographical_area_repository.h
  quota_repository.h

src/domain/
  commodity_aggregate.h
  goods_nomenclature_tree.h
  measure_graph.h
  jsonapi_include_index.h

src/loaders/
  commodity_loader.h
  commodity_include_loader.h
  goods_nomenclature_tree_loader.h
  measure_graph_loader.h

src/renderers/
  jsonapi_renderer.h
  commodity_jsonapi_renderer.h
  measure_jsonapi_renderer.h
  goods_nomenclature_jsonapi_renderer.h
```

Do not create all of this at once. Introduce modules only when moving a real response slice.

## Core Types

### Request Context

Create one request context type and pass it through loaders.

```c
typedef struct {
    const char *service;      /* "uk" or "xi", borrowed */
    const char *commodity_id; /* ten digits, borrowed */
    const char *actual_date;  /* YYYY-MM-DD or NULL, borrowed */
} CommodityRequestContext;
```

The context owns nothing. It should be valid only for the request.

### Include Plan

Represent JSON:API include paths as explicit flags or ids.

Start simple:

```c
typedef struct {
    int include_heading;
    int include_chapter;
    int include_ancestors;

    int include_import_measures;
    int include_export_measures;
    int include_measure_components;
    int include_resolved_measure_components;
    int include_measure_conditions;
    int include_footnotes;
    int include_geographical_areas;
    int include_additional_codes;
    int include_quota_definitions;
} CommodityIncludePlan;
```

Later, if needed, replace flags with a compact enum bitset:

```c
typedef enum {
    COMMODITY_INCLUDE_HEADING = 0,
    COMMODITY_INCLUDE_CHAPTER,
    COMMODITY_INCLUDE_ANCESTORS,
    COMMODITY_INCLUDE_IMPORT_MEASURES,
    COMMODITY_INCLUDE_EXPORT_MEASURES,
    COMMODITY_INCLUDE_MEASURE_COMPONENTS,
    COMMODITY_INCLUDE_MEASURE_CONDITIONS,
} CommodityIncludeId;
```

Parse unsupported includes into a JSON:API error. Do not silently ignore them.

### Include Registry

Use a static table for supported include paths.

```c
typedef struct {
    const char *path;
    int (*enable)(CommodityIncludePlan *plan);
} CommodityIncludeSpec;
```

Examples:

```text
heading
chapter
ancestors
import_measures
import_measures.measure_components
import_measures.resolved_measure_components
import_measures.resolved_measure_components.measurement_unit
export_measures
export_measures.measure_components
```

Default Rails includes should be represented as a predefined plan. Request includes can be layered on top later.

### Typed Aggregate

The aggregate is the endpoint read model. It owns all loaded data for the response.

Sketch:

```c
typedef struct {
    CommodityModel commodity;
    GoodsNomenclatureList ancestors;
    GoodsNomenclatureModel *heading;
    ChapterModel *chapter;

    MeasureList measures;
    MeasureComponentList measure_components;
    MeasureConditionList measure_conditions;
    FootnoteList footnotes;
    GeographicalAreaList geographical_areas;
    AdditionalCodeList additional_codes;
    QuotaOrderNumberList quota_order_numbers;
    QuotaDefinitionList quota_definitions;

    JsonApiIncludeIndex included_index;
} CommodityAggregate;
```

Every list needs:

- `items`
- `len`
- `cap`
- `free` function

Every model that owns heap strings needs a matching `*_free` function.

## Repositories

Repositories should return row-shaped data or typed lists. Do not return rendered JSON.

Good repository API:

```c
int measure_repository_fetch_for_goods_nomenclature_sids(
    const CommodityRequestContext *ctx,
    const char *const *goods_nomenclature_sids,
    size_t sid_count,
    MeasureList *out
);
```

Avoid:

```c
char *measure_repository_render_json(...);
```

### Batched Loading

Prefer batched queries:

```sql
WHERE goods_nomenclature_sid = ANY($1)
```

or:

```sql
JOIN unnest($1::bigint[]) AS requested_sid(goods_nomenclature_sid)
```

Do not fetch children one parent at a time.

### SQL Storage

Move non-trivial SQL out of C string concatenation.

Preferred:

```text
sql/commodity/find.sql
sql/commodity/ancestors.sql
sql/commodity/measures_for_goods_nomenclatures.sql
sql/commodity/measure_components_for_measures.sql
```

Load SQL files in development, or generate an embedded header for production builds. The important point is that SQL should be formatted and reviewable as SQL.

## Nested Set Loader

Rails implementation to match:

- `../app/models/goods_nomenclatures/nested_set.rb`
- `../app/models/goods_nomenclatures/tree_node.rb`
- `../docs/goods-nomenclature-nested-set.md`

The C implementation should use `goods_nomenclature_tree_nodes` as the read model.

### Required Operations

Implement these as explicit repository functions:

```c
int goods_tree_repository_fetch_origin(
    const CommodityRequestContext *ctx,
    GoodsNomenclatureTreeNode *out
);

int goods_tree_repository_fetch_ancestors(
    const CommodityRequestContext *ctx,
    const GoodsNomenclatureTreeNode *origin,
    GoodsNomenclatureList *out
);

int goods_tree_repository_fetch_descendants(
    const CommodityRequestContext *ctx,
    const GoodsNomenclatureTreeNode *origin,
    GoodsNomenclatureList *out
);

int goods_tree_repository_fetch_children(
    const CommodityRequestContext *ctx,
    const GoodsNomenclatureTreeNode *origin,
    GoodsNomenclatureList *out
);
```

The repository should apply:

- `actual_date` validity filtering
- hidden goods filtering where Rails does
- `position` ordering
- `depth` constraints
- `number_indents`
- `leaf` calculation where needed

### Tree Assembler

Mirror the Rails populators explicitly:

```c
int goods_tree_attach_parent_from_ancestors(
    CommodityAggregate *aggregate
);

int goods_tree_attach_children_from_descendants(
    GoodsNomenclatureModel *origin,
    GoodsNomenclatureList *descendants
);

int goods_tree_attach_ancestors_to_descendants_when_loaded(
    GoodsNomenclatureModel *origin
);
```

This replaces the Rails `recursive_ancestor_populator` and `recursive_descendant_populator` behaviour.

The assembler should not query the database.

### Commodity Applicable Measures

Rails no longer relies on naive nested eager loading for commodity measures. It loads commodity plus ancestors, then batches measures for all relevant goods nomenclature SIDs.

Copy that shape:

```text
load commodity
load ancestors
collect [commodity.sid] + ancestor.sids
load all direct measures for those SIDs
attach direct measures to each goods nomenclature
derive applicable measures = ancestors.direct_measures + commodity.direct_measures
```

Quota loading should remain conditional:

```text
if no applicable measure has ordernumber:
  skip quota queries
else:
  load quota order numbers and definitions in one batch
```

## JSON:API Rendering

Use `yyjson` to build the response document.

The renderer should receive:

```c
char *commodity_jsonapi_render(
    const CommodityAggregate *aggregate,
    const CommodityIncludePlan *plan
);
```

It should not know how to query.

### Included Resource Deduplication

JSON:API `included` must not contain duplicate `(type, id)` resources.

Add an include index:

```c
typedef struct {
    char *type;
    char *id;
} JsonApiResourceKey;

typedef struct {
    JsonApiResourceKey *items;
    size_t len;
    size_t cap;
} JsonApiIncludeIndex;
```

Start with a linear scan. If included resources become large, replace with a hash table later.

### Relationship Linkage

Render relationship linkage independently from inclusion.

For example, a measure can have:

```json
"relationships": {
  "measure_components": {
    "data": [
      { "type": "measure_component", "id": "..." }
    ]
  }
}
```

even if `measure_components` is not in `included`.

This distinction matters for JSON:API include flexibility.

## Migration Sequence

### Phase 0: Freeze Current Behaviour

Before migrating behaviour, keep a stable parity gate.

Required:

- `make test`
- `make parity-smoke`
- at least one representative UK commodity with measures, ancestors, components, quotas if available
- at least one XI commodity

Keep the current SQL renderer as the fallback/oracle during the migration.

### Phase 1: Move Giant SQL Into SQL Files

Move the current SQL template out of `commodity_json_renderer.c`.

Target:

```text
sql/legacy/commodity_response_legacy.sql
src/sql_loader.c
src/sql_loader.h
```

This phase should not change behaviour.

Acceptance:

- generated SQL is the same after substituting schema/date
- existing parity smoke still passes
- `commodity_json_renderer.c` becomes orchestration rather than a huge string literal

### Phase 2: Introduce Include Plan

Add parsing for default includes, but continue rendering through the legacy SQL.

Acceptance:

- default include plan matches Rails `CachedCommodityService::DEFAULT_INCLUDES`
- unsupported request includes return a structured error when query param support is enabled
- query params can remain rejected until the implementation is ready

### Phase 3: Typed Commodity And Ancestors

Add:

- `CommodityModel`
- `GoodsNomenclatureModel`
- `GoodsNomenclatureList`
- `goods_tree_repository_fetch_origin`
- `goods_tree_repository_fetch_ancestors`

Render only the root commodity plus ancestors through typed yyjson in a comparison test.

Do not delete legacy SQL yet.

Acceptance:

- typed renderer matches the legacy SQL for root commodity attributes
- typed renderer matches ancestor linkage and included ancestor resources
- actual-date behaviour matches Rails

### Phase 4: Direct And Applicable Measures

Add a measure repository over the stable `measures` read model.

Implement:

```text
load direct measures for commodity and ancestors in one batch
attach direct measures by goods_nomenclature_sid
derive applicable measures
split import/export measures
```

Acceptance:

- query count does not grow with ancestor count
- import/export relationship linkage matches Rails
- ordering matches Rails
- excluded measure types match Rails
- dedupe-similar behaviour matches Rails

### Phase 5: Measure Component Slice

Move measure components from legacy SQL/postprocessing into typed loading and rendering.

This is a good first deep relationship because the C app already has partial measure-component repository code.

Acceptance:

- components are fetched once for all applicable measure SIDs
- `measure_components` and `resolved_measure_components` relationship shape matches Rails
- measurement unit linkage is present
- no string patching is needed for this slice

### Phase 6: Conditions, Footnotes, Areas, Additional Codes

Move each relationship family independently:

1. measure conditions and condition components
2. footnotes
3. geographical areas and excluded geographical areas
4. additional codes
5. regulations/legal acts

Acceptance for each family:

- batched repository query
- typed model/list
- attach by key
- renderer unit tests
- parity smoke with a commodity exercising the family

### Phase 7: Quotas

Implement conditional quota loading after applicable measures are available.

Acceptance:

- no quota queries when no applicable measure has `ordernumber`
- quota order numbers are fetched in one batch
- quota definitions and events are fetched in bounded batches
- included resource deduplication is correct

### Phase 8: Remove Legacy Renderer

Only remove the legacy SQL renderer when typed rendering covers:

- root commodity
- heading/chapter/section
- ancestors
- measures
- measure components
- measure conditions
- footnotes
- geographical areas
- additional codes
- quotas
- import trade summary
- duty calculator metadata

Acceptance:

- `make test`
- `make parity-smoke`
- exhaustive UK parity command documented in README
- no response string postprocessor for migrated slices

## Testing Strategy

### Unit Tests

Add unit tests for:

- include path parsing
- unsupported include errors
- list append/free behaviour
- model free functions
- JSON:API included deduplication
- tree assembler parent/children/ancestor propagation
- renderer output for small hand-built aggregates

### Repository Tests

Repository tests can run against the local tariff database.

Focus on query shape and edge cases:

- actual date filters
- hidden goods
- ancestor ordering
- child/descendant boundaries
- no N+1 query loops

### Parity Tests

Every migrated slice needs parity coverage.

When a bug is found, add the commodity id to:

```text
tests/parity-smoke/uk.txt
tests/parity-smoke/xi.txt
```

Use Rails as oracle until typed C fully owns the response.

### Sanitizers

Run sanitizer targets regularly:

```sh
make test-asan
make ubsan
```

Typed aggregates increase heap ownership surface, so sanitizers are not optional for this migration.

## Risks

### Rebuilding Sequel accidentally

Do not create a generic ORM. The endpoint needs a small include planner, batched repositories, and a few domain-specific assemblers.

### Losing Rails ordering semantics

A lot of parity depends on order:

- ancestors by tree position
- measures by Rails sort rules
- components by duty expression/component order
- included resources in stable serializer order

Capture ordering explicitly in repository SQL or renderer sort steps.

### Ownership bugs

Every typed model/list must have clear ownership.

Rules:

- `char *` fields are owned unless documented otherwise
- `const char *` fields are borrowed
- list free functions free nested items
- aggregate free function frees everything exactly once

### Hidden lazy loading

No loader should be called from a presenter or renderer. Keep this rule strict.

### Over-isolating SQL

Moving SQL from C strings into `.sql` files is not enough. It improves readability, but it does not fix the architecture by itself.

The real improvement comes when SQL returns rows and C owns assembly/rendering.

## Immediate Next Work

1. Refresh parity oracles: `DATABASE_URL=… make generate-parity-oracles`, then `make parity-smoke`.
2. Fix parity failures commodity-by-commodity with `make parity-one` (see `docs/implementation-qa.md` checklist).
3. Add each fixed commodity to `tests/parity-smoke/<service>.txt` with a short reason.
4. Extract remaining duty-calculator / formatting rules from `commodity_tree_slice_renderer.c` into named presenters with unit tests.
5. Delete `sql/legacy/commodity_response_legacy.sql` only after exhaustive parity no longer needs it.
