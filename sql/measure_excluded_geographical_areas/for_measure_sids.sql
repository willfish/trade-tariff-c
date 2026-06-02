WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
)
SELECT
  mega.measure_sid::text,
  ga.geographical_area_sid::text,
  mega.excluded_geographical_area
FROM {{SCHEMA}}.measure_excluded_geographical_areas mega
JOIN requested_sids rs
  ON rs.measure_sid = mega.measure_sid
LEFT JOIN {{SCHEMA}}.geographical_areas ga
  ON ga.geographical_area_id = mega.excluded_geographical_area
 AND ga.validity_start_date <= {{ACTUAL_DATE}}
 AND (ga.validity_end_date IS NULL OR ga.validity_end_date >= {{ACTUAL_DATE}})
ORDER BY mega.measure_sid, mega.excluded_geographical_area;
