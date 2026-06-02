#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
backend_root="$(cd "$repo_root/.." && pwd)"

service="${SERVICE:-uk}"
port="${PORT:-18080}"
database_url="${DATABASE_URL:-postgres:///tariff_development}"

if [[ "$service" != "uk" && "$service" != "xi" ]]; then
  echo "SERVICE must be uk or xi" >&2
  exit 2
fi

for command in curl direnv ruby; do
  if ! command -v "$command" >/dev/null; then
    echo "Missing required command: $command" >&2
    exit 2
  fi
done

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
  > "$repo_root/build/parity-server.out" 2> "$repo_root/build/parity-server.err" &
server_pid="$!"

for _ in {1..100}; do
  if curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done

if ! curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
  echo "C server did not become healthy on port $port" >&2
  cat "$repo_root/build/parity-server.err" >&2 || true
  exit 1
fi

(
  cd "$backend_root"
  if [[ "${PARITY_ORACLE_MODE:-auto}" == "read" ]]; then
    SERVICE="$service" \
      C_BASE_URL="http://localhost:$port" \
      PARITY_FAILURE_DIR="$repo_root/build/parity-failures/$service" \
      ruby "$repo_root/scripts/parity_all_commodities.rb"
  else
    SERVICE="$service" \
      C_BASE_URL="http://localhost:$port" \
      PARITY_FAILURE_DIR="$repo_root/build/parity-failures/$service" \
      direnv exec . bundle exec rails runner "$repo_root/scripts/parity_all_commodities.rb"
  fi
)
