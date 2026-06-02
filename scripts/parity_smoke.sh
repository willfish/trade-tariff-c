#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

service="${SERVICE:-uk}"
port="${PORT:-18080}"
database_url="${DATABASE_URL:-postgres:///tariff_development}"
actual_date="${ACTUAL_DATE:-}"
fixture_file="${PARITY_SMOKE_FILE:-$repo_root/tests/parity-smoke/$service.txt}"
required="${PARITY_SMOKE_REQUIRED:-1}"

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

if [[ ! -f "$fixture_file" ]]; then
  if [[ "$required" == "1" ]]; then
    echo "Missing parity smoke fixture file: $fixture_file" >&2
    exit 1
  fi
  echo "Skipping parity smoke: missing fixture file $fixture_file" >&2
  exit 0
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
  > "$repo_root/build/parity-smoke-server.out" 2> "$repo_root/build/parity-smoke-server.err" &
server_pid="$!"

for _ in {1..100}; do
  if curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
    break
  fi
  sleep 0.1
done

if ! curl --silent --fail "http://localhost:$port/health" >/dev/null 2>&1; then
  echo "C server did not become healthy on port $port" >&2
  cat "$repo_root/build/parity-smoke-server.err" >&2 || true
  exit 1
fi

checked=0
skipped=0

while read -r commodity_id _reason; do
  case "$commodity_id" in
    ""|\#*) continue ;;
  esac

  oracle_path=""
  if [[ -n "$actual_date" ]]; then
    candidate="$repo_root/build/parity-oracle/$service/$actual_date/$commodity_id.json"
    [[ -f "$candidate" ]] && oracle_path="$candidate"
  else
    oracle_path="$(find "$repo_root/build/parity-oracle/$service" -maxdepth 2 -type f -name "$commodity_id.json" 2>/dev/null | sort | tail -1 || true)"
  fi

  if [[ -z "$oracle_path" ]]; then
    if [[ "$required" == "1" ]]; then
      echo "Missing oracle snapshot for $service commodity $commodity_id" >&2
      exit 1
    fi
    skipped=$((skipped + 1))
    continue
  fi

  oracle_dir="$(dirname "$oracle_path")"
  SERVICE="$service" \
    COMMODITY_ID="$commodity_id" \
    C_BASE_URL="http://localhost:$port" \
    PARITY_ORACLE_MODE=read \
    PARITY_ORACLE_DIR="$oracle_dir" \
    ruby "$repo_root/scripts/parity_one_commodity.rb"
  checked=$((checked + 1))
done < "$fixture_file"

if [[ "$checked" -eq 0 ]]; then
  if [[ "$required" == "1" ]]; then
    echo "No parity smoke cases had oracle snapshots" >&2
    exit 1
  fi
  echo "Skipping parity smoke: no oracle snapshots available" >&2
  exit 0
fi

echo "Parity smoke passed for SERVICE=$service: checked=$checked skipped=$skipped"
