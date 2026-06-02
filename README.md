# Trade Tariff C

A small C JSON:API backend for one Trade Tariff vertical: loading a commodity by code from the `uk` or `xi` PostgreSQL schema.

This is not a full port of `trade-tariff-backend`. It reproduces the first useful slice:

- `GET /api/commodities/:id`
- `GET /uk/api/commodities/:id`
- `GET /xi/api/commodities/:id`

The selected service maps directly to the PostgreSQL schema. If the route contains `/uk` or `/xi`, that wins. Otherwise `SERVICE` is used, defaulting to `uk`.

There is intentionally no caching. Each commodity request executes a fresh database query.

## Build

```sh
make
```

The standalone Nix shell includes the compiler and library dependencies:

```sh
nix develop -c make test
```

CI runs the sanitizer-backed suite:

```sh
nix develop -c make test-asan
```

## Run

Populate the local database from the merged production dump (credentials from the team secrets store, not committed here):

```sh
curl --silent -u "${TARIFF_DUMP_USER}:${TARIFF_DUMP_PASSWORD}" \
  https://dumps.trade-tariff.service.gov.uk/tariff-merged-production.sql.gz \
  | gunzip | psql tariff_development
```

```sh
DATABASE_URL=postgres://postgres@localhost/tariff_development SERVICE=uk PORT=8080 make run
```

Then:

```sh
curl http://localhost:8080/uk/api/commodities/0101210000
```

The process uses `libpq`. `DATABASE_URL` is used when present; otherwise normal `PGHOST`, `PGUSER`, `PGDATABASE`, and related PostgreSQL environment variables apply.

## Endpoint Contract

The only supported endpoint contract is exact response parity with Rails for:

- `GET /api/commodities/:id`
- `GET /uk/api/commodities/:id`
- `GET /xi/api/commodities/:id`

This maps to the Rails path:

`CommoditiesController#find_commodity -> CachedCommodityService -> CommodityPresenter -> JSONAPI serializer -> optional ResponseFilter`.

Supported query parameters (others return `400` with a JSON:API error):

- `include=` — parsed against the static registry in `commodity_include_plan.c` (Rails default includes when omitted)
- `filter[meursing_additional_code_id]=` — passed through to the typed request context

Duty-calculator metadata uses overlay JSON from `db/additional_codes.json` and `db/measurement_units.json` (copied from `trade-tariff-backend/db/`; see `db/README.md`).

There is intentionally no caching. Each commodity request executes fresh database reads.

## Scope

The response follows JSON:API resource shape for a commodity and is being moved toward named C phases matching the Rails boundaries:

- lookup commodity
- load typed relationship rows
- compute domain projections
- render JSON:API
- compare against Rails oracle

The SQL is schema-qualified and only accepts `uk` or `xi` as service names.

For guided review notes, see `docs/implementation-qa.md`. For C-specific idioms and footguns, see `docs/c-idioms.md`. For the proposed typed replacement architecture, see `docs/typed-architecture-plan.md`.

## Parity Gate

Full commodity parity means byte-for-byte equivalent JSON after stable key sorting against the Rails backend response for the same service and commodity.

```sh
SERVICE=uk COMMODITY_ID=8207809000 make parity
SERVICE=xi COMMODITY_ID=8207809000 make parity
```

Regression checks should be added to `tests/parity-smoke/<service>.txt` whenever a parity failure is discovered and fixed. `make test` runs C self-tests plus read-mode smoke parity for any local oracle snapshots. `make parity-smoke` requires every listed smoke case to have an oracle snapshot and pass.

Generate oracle snapshots (needs `trade-tariff-backend`, PostgreSQL, and direnv):

```sh
DATABASE_URL=postgres://postgres@localhost/tariff_development make generate-parity-oracles
make parity-smoke
```

Set `TRADE_TARIFF_BACKEND` if the Rails app is not at `../hmrc/trade-tariff-backend`.

Full exhaustive UK parity is still the completion gate:

```sh
SERVICE=uk PARITY_ORACLE_MODE=auto ./scripts/parity_all_commodities.sh
```

That command is expected to fail until the C implementation covers the full Rails serializer graph: relationships, included resources, measures, duty calculator metadata, and import trade summary.
