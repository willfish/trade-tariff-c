#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
backend_root="${TRADE_TARIFF_BACKEND:-$repo_root/../hmrc/trade-tariff-backend}"

service="${SERVICE:-uk}"
commodity_id="${COMMODITY_ID:-8207809000}"
port="${PORT:-18080}"
database_url="${DATABASE_URL:-postgres:///tariff_development}"

if [[ "$service" != "uk" && "$service" != "xi" ]]; then
  echo "SERVICE must be uk or xi" >&2
  exit 2
fi

for command in jq curl diff direnv; do
  if ! command -v "$command" >/dev/null; then
    echo "Missing required command: $command" >&2
    exit 2
  fi
done

tmpdir="$(mktemp -d)"
server_pid=""
cleanup() {
  if [[ -n "$server_pid" ]]; then
    kill "$server_pid" 2>/dev/null || true
    wait "$server_pid" 2>/dev/null || true
  fi
  rm -rf "$tmpdir"
}
trap cleanup EXIT

make -C "$repo_root" >/dev/null

(
  cd "$backend_root"
  SERVICE="$service" COMMODITY_ID="$commodity_id" direnv exec . bundle exec rails runner '
    require "json"

    Rails.cache.clear
    TimeMachine.at(Time.zone.today) do
      commodity = Commodity.actual.non_hidden.declarable.by_code(ENV.fetch("COMMODITY_ID")).take
      raise "commodity not found" unless commodity

      response = CachedCommodityService.new(commodity, Time.zone.today, {}).call
      puts JSON.generate(response)
    end
  '
) > "$tmpdir/rails.json"

DATABASE_URL="$database_url" SERVICE="$service" PORT="$port" "$repo_root/build/trade-tariff-c" \
  > "$tmpdir/server.out" 2> "$tmpdir/server.err" &
server_pid="$!"

for _ in {1..50}; do
  if curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done

curl --silent --show-error --fail "http://localhost:$port/$service/api/commodities/$commodity_id" > "$tmpdir/c.json"

jq -S . "$tmpdir/rails.json" > "$tmpdir/rails.sorted.json"
jq -S . "$tmpdir/c.json" > "$tmpdir/c.sorted.json"

if ! diff -u "$tmpdir/rails.sorted.json" "$tmpdir/c.sorted.json" > "$tmpdir/diff"; then
  echo
  echo "Parity failed for SERVICE=$service COMMODITY_ID=$commodity_id" >&2
  echo "Rails bytes: $(wc -c < "$tmpdir/rails.json")" >&2
  echo "C bytes:     $(wc -c < "$tmpdir/c.json")" >&2
  echo "Rails summary:" >&2
  jq -c '{relationships: ((.data.relationships // {}) | keys), included_count: (.included | length // 0), included_types: ([.included[]?.type] | group_by(.) | map({type: .[0], count: length}))}' "$tmpdir/rails.json" >&2
  echo "C summary:" >&2
  jq -c '{relationships: ((.data.relationships // {}) | keys), included_count: (.included | length // 0), included_types: ([.included[]?.type] | group_by(.) | map({type: .[0], count: length}))}' "$tmpdir/c.json" >&2
  echo "First diff lines:" >&2
  sed -n '1,220p' "$tmpdir/diff" >&2
  exit 1
fi

echo "Parity passed for SERVICE=$service COMMODITY_ID=$commodity_id"
