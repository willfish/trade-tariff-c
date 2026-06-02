WITH requested_codes AS (
  SELECT requested.measurement_unit_code, requested.source_order
  FROM unnest($1::text[]) WITH ORDINALITY AS requested(measurement_unit_code, source_order)
  WHERE requested.measurement_unit_code IS NOT NULL
    AND requested.measurement_unit_code <> ''
)
SELECT DISTINCT ON (mu.measurement_unit_code)
  mu.measurement_unit_code,
  mud.description,
  mua.abbreviation
FROM requested_codes requested
JOIN {{SCHEMA}}.measurement_units mu
  ON mu.measurement_unit_code = requested.measurement_unit_code
LEFT JOIN {{SCHEMA}}.measurement_unit_descriptions mud
  ON mud.measurement_unit_code = mu.measurement_unit_code
LEFT JOIN {{SCHEMA}}.measurement_unit_abbreviations mua
  ON mua.measurement_unit_code = mu.measurement_unit_code
 AND mua.measurement_unit_qualifier IS NULL
ORDER BY mu.measurement_unit_code, requested.source_order;
