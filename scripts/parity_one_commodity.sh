#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
backend_root="$(cd "$repo_root/.." && pwd)"

service="${SERVICE:-uk}"
commodity_id="${COMMODITY_ID:?COMMODITY_ID is required}"
port="${PORT:-18080}"
database_url="${DATABASE_URL:-postgres:///tariff_development}"
oracle_mode="${PARITY_ORACLE_MODE:-read}"

if [[ "$service" != "uk" && "$service" != "xi" ]]; then
  echo "SERVICE must be uk or xi" >&2
  exit 2
fi

for command in curl ruby; do
  if ! command -v "$command" >/dev/null; then
    echo "Missing required command: $command" >&2
    exit 2
  fi
done

if [[ "$oracle_mode" != "read" ]] && ! command -v direnv >/dev/null; then
  echo "Missing required command: direnv" >&2
  exit 2
fi

server_pid=""
cleanup() {
  if [[ -n "$server_pid" ]]; then
    kill "$server_pid" 2>/dev/null || true
    wait "$server_pid" 2>/dev/null || true
  fi
}
trap cleanup EXIT

make -C "$repo_root" >/dev/null

DATABASE_URL="$database_url" SERVICE="$service" PORT="$port" "$repo_root/build/trade-tariff-c" \
  > "$repo_root/build/parity-one-server.out" 2> "$repo_root/build/parity-one-server.err" &
server_pid="$!"

for _ in {1..100}; do
  if curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done

if ! curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
  echo "C server did not become healthy on port $port" >&2
  cat "$repo_root/build/parity-one-server.err" >&2 || true
  exit 1
fi

if [[ "$oracle_mode" == "read" ]]; then
  SERVICE="$service" \
    COMMODITY_ID="$commodity_id" \
    C_BASE_URL="http://localhost:$port" \
    PARITY_ORACLE_MODE="$oracle_mode" \
    ruby "$repo_root/scripts/parity_one_commodity.rb"
else
  (
    cd "$backend_root"
    SERVICE="$service" \
      COMMODITY_ID="$commodity_id" \
      C_BASE_URL="http://localhost:$port" \
      PARITY_ORACLE_MODE="$oracle_mode" \
      direnv exec . bundle exec rails runner "$repo_root/scripts/parity_one_commodity.rb"
  )
fi
