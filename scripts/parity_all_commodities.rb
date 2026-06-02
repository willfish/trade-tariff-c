require "fileutils"
require "date"
require "json"
require "net/http"
require "time"

service = ENV.fetch("SERVICE", "uk")
base_url = ENV.fetch("C_BASE_URL")
comms_path = ENV.fetch("COMMS_JSON", File.expand_path("../../comms.json", __dir__))
failure_dir = ENV.fetch("PARITY_FAILURE_DIR", File.expand_path("../build/parity-failures/#{service}", __dir__))
oracle_mode = ENV.fetch("PARITY_ORACLE_MODE", "auto")
actual_date = ENV.fetch("ACTUAL_DATE", Date.today.iso8601)
oracle_dir = ENV.fetch("PARITY_ORACLE_DIR", File.expand_path("../build/parity-oracle/#{service}/#{actual_date}", __dir__))
limit = ENV["LIMIT"]&.to_i
offset = ENV.fetch("OFFSET", "0").to_i
fail_fast = ENV.fetch("FAIL_FAST", "0") == "1"

raise "SERVICE must be uk or xi" unless %w[uk xi].include?(service)
raise "PARITY_ORACLE_MODE must be auto, read, or refresh" unless %w[auto read refresh].include?(oracle_mode)

FileUtils.rm_rf(failure_dir)
FileUtils.mkdir_p(failure_dir)
FileUtils.mkdir_p(oracle_dir) unless oracle_mode == "read"

all_cases = JSON.parse(File.read(comms_path))
cases = all_cases.drop(offset)
cases = cases.first(limit) if limit&.positive?

started_at = Time.now
failures = []
checked = 0

def sorted_json(value, key = nil)
  case value
  when Hash
    value.keys.sort.each_with_object({}) { |child_key, output| output[child_key] = sorted_json(value[child_key], child_key) }
  when Array
    normalized = value.map { |entry| sorted_json(entry) }
    if key == "included" && normalized.all? { |entry| entry.is_a?(Hash) && entry.key?("type") && entry.key?("id") }
      normalized.sort_by { |entry| [entry.fetch("type"), entry.fetch("id")] }
    else
      normalized
    end
  else
    value
  end
end

def write_json(path, value)
  File.write(path, JSON.pretty_generate(sorted_json(value)))
end

def rails_available?
  defined?(Commodity) && defined?(CachedCommodityService) && defined?(TimeMachine) && defined?(Time.zone)
end

def load_rails_json(commodity_id)
  commodity = Commodity.actual.non_hidden.declarable.by_code(commodity_id).take
  return nil unless commodity

  JSON.parse(JSON.generate(CachedCommodityService.new(commodity, Time.zone.today, {}).call))
end

if oracle_mode != "read" && !rails_available?
  raise "PARITY_ORACLE_MODE=#{oracle_mode} requires running under rails runner"
end

runner = proc do
  cases.each_with_index do |(sid, commodity_id), index|
    checked += 1

    oracle_path = File.join(oracle_dir, "#{commodity_id}.json")
    rails_json = nil

    if oracle_mode != "refresh" && File.exist?(oracle_path)
      rails_json = JSON.parse(File.read(oracle_path))
    elsif oracle_mode == "read"
      failures << [sid, commodity_id, "missing oracle snapshot"]
      warn "#{commodity_id}: missing oracle snapshot at #{oracle_path}"
      break if fail_fast
      next
    else
      rails_json = load_rails_json(commodity_id)
      unless rails_json
        failures << [sid, commodity_id, "rails commodity not found"]
        warn "#{commodity_id}: Rails commodity not found"
        break if fail_fast
        next
      end
      write_json(oracle_path, rails_json)
    end

    uri = URI("#{base_url}/#{service}/api/commodities/#{commodity_id}")
    response = Net::HTTP.get_response(uri)
    unless response.is_a?(Net::HTTPSuccess)
      failures << [sid, commodity_id, "c http #{response.code}"]
      warn "#{commodity_id}: C HTTP #{response.code}"
      break if fail_fast
      next
    end

    c_json = JSON.parse(response.body)
    if sorted_json(rails_json) != sorted_json(c_json)
      failures << [sid, commodity_id, "json mismatch"]
      case_dir = File.join(failure_dir, commodity_id)
      FileUtils.mkdir_p(case_dir)
      write_json(File.join(case_dir, "rails.json"), rails_json)
      write_json(File.join(case_dir, "c.json"), c_json)
      warn "#{commodity_id}: JSON mismatch"
      break if fail_fast
    end

    if ((index + 1) % 100).zero?
      elapsed = Time.now - started_at
      warn "checked=#{checked} failures=#{failures.length} elapsed=#{elapsed.round(1)}s"
    end
  end
end

if oracle_mode == "read"
  runner.call
else
  TimeMachine.at(Time.zone.today) do
    runner.call
  end
end

summary = {
  service: service,
  actual_date: actual_date,
  oracle_mode: oracle_mode,
  oracle_dir: oracle_dir,
  checked: checked,
  failed: failures.length,
  total_available: all_cases.length,
  offset: offset,
  limit: limit,
  failures: failures.map { |sid, commodity_id, reason| { sid: sid, id: commodity_id, reason: reason } }
}

summary_path = File.join(failure_dir, "summary.json")
File.write(summary_path, JSON.pretty_generate(summary))

if failures.any?
  warn "Parity failed: #{failures.length}/#{checked}. Summary: #{summary_path}"
  exit 1
end

puts "Parity passed for SERVICE=#{service}: #{checked}/#{all_cases.length} commodities"
