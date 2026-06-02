require "date"
require "fileutils"
require "json"
require "net/http"

service = ENV.fetch("SERVICE", "uk")
commodity_id = ENV.fetch("COMMODITY_ID")
base_url = ENV.fetch("C_BASE_URL")
oracle_mode = ENV.fetch("PARITY_ORACLE_MODE", "auto")
actual_date = ENV.fetch("ACTUAL_DATE", Date.today.iso8601)
repo_root = File.expand_path("..", __dir__)
oracle_dir = ENV.fetch("PARITY_ORACLE_DIR", File.join(repo_root, "build/parity-oracle/#{service}/#{actual_date}"))
failure_dir = ENV.fetch("PARITY_FAILURE_DIR", File.join(repo_root, "build/parity-failures/#{service}/#{commodity_id}"))

raise "SERVICE must be uk or xi" unless %w[uk xi].include?(service)
raise "PARITY_ORACLE_MODE must be auto, read, or refresh" unless %w[auto read refresh].include?(oracle_mode)

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
  FileUtils.mkdir_p(File.dirname(path))
  File.write(path, JSON.pretty_generate(sorted_json(value)))
end

def rails_available?
  defined?(Commodity) && defined?(CachedCommodityService) && defined?(TimeMachine) && defined?(Time.zone)
end

def load_rails_json(commodity_id, actual_date)
  TimeMachine.at(Date.parse(actual_date)) do
    commodity = Commodity.actual.non_hidden.declarable.by_code(commodity_id).take
    return nil unless commodity

    JSON.parse(JSON.generate(CachedCommodityService.new(commodity, Time.zone.today, {}).call))
  end
end

def fetch_c_json(base_url, service, commodity_id)
  uri = URI("#{base_url}/#{service}/api/commodities/#{commodity_id}")
  response = Net::HTTP.get_response(uri)
  raise "C HTTP #{response.code}: #{response.body}" unless response.is_a?(Net::HTTPSuccess)

  JSON.parse(response.body)
end

def included_index(json)
  Array(json["included"]).each_with_object({}) do |resource, index|
    next unless resource.is_a?(Hash)

    index[[resource["type"], resource["id"]]] = resource
  end
end

def first_diff(expected, actual, path = [])
  return nil if expected == actual

  if expected.class != actual.class
    return [path, expected, actual, "type"]
  end

  case expected
  when Hash
    keys = (expected.keys | actual.keys).sort
    keys.each do |key|
      return [path + [key], expected[key], actual[key], "missing"] unless expected.key?(key) && actual.key?(key)

      diff = first_diff(expected[key], actual[key], path + [key])
      return diff if diff
    end
  when Array
    return [path + ["length"], expected.length, actual.length, "length"] if expected.length != actual.length

    expected.each_with_index do |entry, index|
      diff = first_diff(entry, actual[index], path + [index])
      return diff if diff
    end
  else
    [path, expected, actual, "value"]
  end
end

def json_path(path)
  path.reduce("$") do |output, part|
    part.is_a?(Integer) ? "#{output}[#{part}]" : "#{output}.#{part}"
  end
end

def resource_for_path(json, path)
  return ["data", json["data"]] if path.first == "data"
  return nil unless path.first == "included" && path[1].is_a?(Integer)

  resource = Array(json["included"])[path[1]]
  return nil unless resource.is_a?(Hash)

  ["#{resource["type"]}:#{resource["id"]}", resource]
end

if oracle_mode != "read" && !rails_available?
  raise "PARITY_ORACLE_MODE=#{oracle_mode} requires running under rails runner"
end

oracle_path = File.join(oracle_dir, "#{commodity_id}.json")
rails_json =
  if oracle_mode != "refresh" && File.exist?(oracle_path)
    JSON.parse(File.read(oracle_path))
  elsif oracle_mode == "read"
    raise "missing oracle snapshot at #{oracle_path}"
  else
    loaded = load_rails_json(commodity_id, actual_date)
    raise "Rails commodity not found: #{commodity_id}" unless loaded

    write_json(oracle_path, loaded)
    loaded
  end

c_json = fetch_c_json(base_url, service, commodity_id)
expected = sorted_json(rails_json)
actual = sorted_json(c_json)

FileUtils.rm_rf(failure_dir)

if expected == actual
  puts "Parity passed for SERVICE=#{service} COMMODITY_ID=#{commodity_id}"
  exit 0
end

FileUtils.mkdir_p(failure_dir)
write_json(File.join(failure_dir, "rails.json"), rails_json)
write_json(File.join(failure_dir, "c.json"), c_json)

diff = first_diff(expected, actual)
path, expected_value, actual_value, kind = diff
resource = resource_for_path(actual, path) || resource_for_path(expected, path)

puts "Parity failed for SERVICE=#{service} COMMODITY_ID=#{commodity_id}"
puts "failure_dir=#{failure_dir}"
puts "first_diff_kind=#{kind}"
puts "first_diff_path=#{json_path(path)}"
puts "resource=#{resource ? resource.first : "unknown"}"
puts "expected=#{JSON.generate(expected_value)}"
puts "actual=#{JSON.generate(actual_value)}"

rails_resources = included_index(expected)
c_resources = included_index(actual)
missing_from_c = rails_resources.keys - c_resources.keys
extra_in_c = c_resources.keys - rails_resources.keys
puts "missing_in_c=#{missing_from_c.first(10).map { |type, id| "#{type}:#{id}" }.join(",")}"
puts "extra_in_c=#{extra_in_c.first(10).map { |type, id| "#{type}:#{id}" }.join(",")}"

exit 1
