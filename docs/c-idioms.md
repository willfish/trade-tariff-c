# C Idioms In This Repo

This guide assumes you already know the Rails trade-tariff domain. It focuses on how to read and change the C safely.

## Mental Model

C does not give you Rails-style object lifetime, exceptions, strings, arrays, hashes, or reflection. Most patterns in this repo are compensating for that:

- ownership is explicit
- errors are values
- strings are byte buffers
- arrays carry their own length/capacity
- modules are header/source pairs
- polymorphism is mostly naming convention, not runtime dispatch
- tests and sanitizers catch mistakes the language will not catch

## File And Module Shape

### Header/source pairs

Most modules have:

- `name.h` - public types and function declarations
- `name.c` - implementation and private `static` helpers

Example:

- `src/renderers/commodity_tree_slice_renderer.h`
- `src/renderers/commodity_tree_slice_renderer.c`

Anything declared `static` in a `.c` file is private to that translation unit. It is not visible to other `.c` files and avoids symbol collisions at link time.

### Include guards

Headers use `#ifndef ... #define ... #endif` guards to prevent duplicate declarations during compilation.

Example:

```c
#ifndef TRADE_TARIFF_C_JSON_PATCH_H
#define TRADE_TARIFF_C_JSON_PATCH_H
...
#endif
```

### Build model

Each `.c` file compiles into an object file. The linker then combines the object files into `build/trade-tariff-c`.

The Makefile's `SRC` list is the source of truth for what enters the binary. If a new `.c` file is not in `SRC`, it will not be linked.

## Ownership

### Owned versus borrowed strings

This is the most important C habit.

An owned pointer must be freed by someone. A borrowed pointer must not be freed.

Common local convention:

- functions returning `char *` usually return owned memory
- functions returning `const char *` usually return borrowed memory or a static string
- structs with `char *` fields usually own those strings
- structs with `const char *` fields usually borrow those strings

Examples:

- `json_escape` returns owned `char *`; caller must `free`.
- `preference_code_presenter_description` returns borrowed static text; caller must not `free`.
- `CommodityRequest` stores borrowed `const char *` request values.
- `DbResult.data` and `DbResult.error` are owned and released by `db_result_free`.

### Free functions

When a type owns heap memory, it should have a matching cleanup function:

- `db_result_free`
- `db_rows_free`
- `footnote_free`
- `footnote_list_free`
- `json_builder_free`
- `free_response`

The cleanup function should leave the object in a harmless zero/null state when practical. That makes repeated cleanup less dangerous and debugging easier.

### Return-and-cleanup pattern

Because C has no exceptions or `ensure`, code tends to look like:

```c
char *value = make_value();
if (value == NULL) {
    return error_response();
}

char *next = transform(value);
free(value);
if (next == NULL) {
    return error_response();
}
```

When a function has many resources, prefer a single cleanup label or small helper functions rather than duplicating frees across every branch. The current code uses both styles.

## Errors

### No exceptions

Errors are returned explicitly.

Common patterns in this repo:

- `NULL` means allocation/parse/build failure for pointer-returning functions.
- `0`/`1` means false/true or failure/success for `int` functions.
- structs include `ok` and `error` fields for richer results.
- HTTP handlers return `HttpResponse` values.

Example:

```c
typedef struct {
    char *data;
    size_t len;
    char *error;
    int ok;
} DbResult;
```

Callers must check `ok` before using `data`.

### `errno` is not enough

Some C/library calls set `errno`; many do not. This repo mostly uses explicit error strings for DB operations and generic JSON:API errors for HTTP responses.

## Strings And Buffers

### C strings are null-terminated byte arrays

`char *` points to bytes ending with `'\0'`. Functions like `strlen`, `strstr`, `strcmp`, and `snprintf` expect that terminator.

If a buffer is not null-terminated, string functions can read past it.

### Prefer `snprintf` sizing

The common safe allocation pattern is:

```c
int needed = snprintf(NULL, 0, template, value);
char *out = malloc((size_t)needed + 1);
snprintf(out, (size_t)needed + 1, template, value);
```

Watch for negative `needed`, integer casts, and multiplication overflow.

### `strdup` is allocation

`strdup` returns owned memory. It must be freed. It is POSIX, not ISO C, so `_POSIX_C_SOURCE=200809L` is enabled in `CPPFLAGS`.

## Arrays And Lists

C arrays do not know their length. Local list structs carry:

- pointer to items
- `len`
- `cap`

Example:

```c
typedef struct {
    MeasureComponentModel *items;
    size_t len;
    size_t cap;
} MeasureComponentModelList;
```

When appending, grow capacity with `realloc`, usually doubling capacity.

Important `realloc` rule: never assign directly to the original pointer until success.

Good:

```c
T *grown = realloc(items, next_cap * sizeof(*items));
if (grown == NULL) return 0;
items = grown;
```

Bad:

```c
items = realloc(items, next_cap * sizeof(*items)); // leaks original on failure
```

## Structs

### Plain data structs

C structs are data bags. There are no constructors, destructors, methods, visibility modifiers, or inheritance.

This repo uses constructor-like functions where useful:

```c
CommodityRequest commodity_request_new(const char *service, const char *commodity_id);
```

### Zero initialization

This is common and useful:

```c
MeasureComponentModel model = { 0 };
```

It sets pointers to `NULL` and integers to zero.

### Ownership should be obvious from field types

Use `const char *` when the struct borrows a string. Use `char *` when the struct owns it.

If a struct owns fields, add a free function.

## Const Correctness

`const char *` means the function promises not to mutate the pointed-to bytes. It does not necessarily mean the memory is immutable globally.

Use `const` for:

- borrowed request inputs
- static lookup descriptions
- SQL template strings
- read-only JSON input passed into yyjson

Use mutable `char *` when a function edits or owns a buffer.

## yyjson And JSON DOM Work

### Immutable versus mutable yyjson documents

`yyjson_read` returns an immutable parsed document:

```c
yyjson_doc *doc = yyjson_read(json, strlen(json), 0);
```

To mutate, copy it:

```c
yyjson_mut_doc *mut = yyjson_doc_mut_copy(doc, NULL);
```

Then use `yyjson_mut_obj_get`, `yyjson_mut_obj_replace`, `yyjson_mut_arr_append`, etc.

### yyjson memory ownership

Values live inside their document. Do not free individual `yyjson_mut_val *` values.

Free the whole document:

```c
yyjson_doc_free(doc);
yyjson_mut_doc_free(mut);
```

`yyjson_mut_write` returns a newly allocated `char *`. The caller must `free` it.

### Current local use

The commodity response is built directly with yyjson in renderer modules such as
`src/renderers/commodity_tree_slice_renderer.c` and the per-resource JSON:API
renderers. Prefer adding named renderer/presenter functions over parsing and
mutating completed JSON.

## SQL And libpq

### Prepared statements

`db_fetch_single_text` and `db_fetch_rows` prepare a named statement and pass one bound parameter. Commodity id is bound as `$1`.

The schema name is interpolated into the SQL because PostgreSQL cannot bind identifiers. That is why service validation must stay strict.

### Result ownership

libpq result values are copied into owned C strings before returning from `db.c`. That makes the returned `DbRows` independent of the `PGresult`, but it means callers must release with `db_rows_free`.

### Global DB state

`src/db.c` has process-global connection/prepared-statement state. That is simple for a single blocking server, but do not make request handling threaded without redesigning DB ownership.

## HTTP Layer

The HTTP server is deliberately small:

- one blocking `accept`
- one `read`
- fixed-size request buffer
- method/path parse with `sscanf`
- response body length from `strlen`
- `Connection: close`

This is a test/parity harness, not a general HTTP server. If the service goes production, decide whether to keep this behind a process supervisor/load balancer or replace it with a proper HTTP/event library.

## Tests

### Built-in self-tests

Many modules expose `*_self_test(void)`. These are compiled into the binary and run by:

```sh
build/trade-tariff-c --self-test
```

`commodity_service_self_test` composes most of them.

This is lightweight, but it is not as expressive as a unit test framework.

### cmocka tests

`tests/unit` contains normal C unit tests using cmocka. Use these for isolated rules.

Shape:

```c
static void test_name(void **state)
{
    (void)state;
    ...
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_name),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

### Sanitizers

Run sanitizer builds for ownership/string changes:

```sh
make test-asan
```

AddressSanitizer catches leaks, use-after-free, double-free, and out-of-bounds access. UBSan catches undefined behaviour such as invalid integer operations.

## Common C Footguns In This Repo

### Leaking on early return

Every owned pointer in scope must be freed on every exit path.

### Returning borrowed stack memory

Never return a pointer to a local stack array:

```c
char buffer[64];
return buffer; // wrong
```

Return heap memory or write into caller-provided storage.

### Use-after-free

After `free(ptr)`, the pointer value is stale. Set it to `NULL` when the same scope might touch it again.

### Pointer invalidation after replacement

If a function replaces a string buffer, all pointers into the old buffer become invalid.

The safer pattern is to store offsets, replace, then recompute pointers from the new base address.

### JSON string rewriting

Searching for `"type"` or `"description"` in JSON text is brittle. Prefer yyjson when touching JSON structure. If string rewriting is unavoidable, keep the function tiny and add both unit and parity coverage.

### Silent truncation

Fixed-size buffers plus `snprintf` can truncate. Always check the return value when truncation would alter behaviour.

### Integer overflow in allocation sizes

Expressions like `rows * cols` or `len + extra + 1` can overflow before allocation. The current code is small enough that this is not the first risk, but it matters as inputs grow.

## Local Naming Conventions

- `*_new` creates a small value or object.
- `*_free` releases owned memory.
- `*_self_test` runs compiled-in checks.
- `*_presenter_*` maps model fields to JSON/API-facing values.
- `*_repository_*` reads database rows or builds SQL.
- `json_builder_*` builds new JSON using yyjson.

## How To Review A C Change Here

Ask these before reviewing domain correctness:

1. Who owns every pointer returned by the changed function?
2. Is every owned pointer freed on success and failure?
3. Are borrowed pointers documented by type (`const char *`) or naming?
4. Can any pointer into a buffer survive after the buffer is reallocated/replaced?
5. Are null checks present after allocation and parser calls?
6. Are array lengths carried alongside array pointers?
7. Are SQL identifiers still constrained before interpolation?
8. Is JSON changed structurally rather than by string search where possible?
9. Is there a cmocka test for the C rule?
10. Has `make test-asan` been run for memory/string-heavy changes?
