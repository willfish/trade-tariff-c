#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
backend_root="${TRADE_TARIFF_BACKEND:-$repo_root/../hmrc/trade-tariff-backend}"
service="${SERVICE:-uk}"
actual_date="${ACTUAL_DATE:-$(date +%F)}"
port="${PORT:-18080}"
database_url="${DATABASE_URL:-postgres:///tariff_development}"
fixture_file="${PARITY_SMOKE_FILE:-$repo_root/tests/parity-smoke/$service.txt}"

if [[ ! -d "$backend_root" ]]; then
  echo "Rails backend not found at $backend_root (set TRADE_TARIFF_BACKEND)" >&2
  exit 2
fi

if [[ ! -f "$fixture_file" ]]; then
  echo "Missing fixture file: $fixture_file" >&2
  exit 1
fi

make -C "$repo_root" >/dev/null

server_pid=""
cleanup() {
  if [[ -n "$server_pid" ]]; then
    kill "$server_pid" 2>/dev/null || true
    wait "$server_pid" 2>/dev/null || true
  fi
}
trap cleanup EXIT

DATABASE_URL="$database_url" SERVICE="$service" PORT="$port" "$repo_root/build/trade-tariff-c" \
  > "$repo_root/build/parity-oracle-server.out" 2> "$repo_root/build/parity-oracle-server.err" &
server_pid="$!"

for _ in {1..100}; do
  if curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done

if ! curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
  echo "C server did not become healthy on port $port" >&2
  exit 1
fi

generated=0
while read -r commodity_id _reason; do
  case "$commodity_id" in
    ""|\#*) continue ;;
  esac

  (
    cd "$backend_root"
    ACTUAL_DATE="$actual_date" \
      SERVICE="$service" \
      COMMODITY_ID="$commodity_id" \
      C_BASE_URL="http://localhost:$port" \
      PARITY_ORACLE_MODE=refresh \
      PARITY_ORACLE_DIR="$repo_root/build/parity-oracle/$service/$actual_date" \
      direnv exec . bundle exec rails runner "$repo_root/scripts/parity_one_commodity.rb"
  )
  generated=$((generated + 1))
done < "$fixture_file"

echo "Generated $generated oracle snapshot(s) under build/parity-oracle/$service/$actual_date"