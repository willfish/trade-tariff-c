WITH requested_codes AS (
  SELECT requested.measurement_unit_qualifier_code, requested.source_order
  FROM unnest($1::text[]) WITH ORDINALITY AS requested(measurement_unit_qualifier_code, source_order)
  WHERE requested.measurement_unit_qualifier_code IS NOT NULL
    AND requested.measurement_unit_qualifier_code <> ''
)
SELECT DISTINCT ON (muq.measurement_unit_qualifier_code)
  muq.measurement_unit_qualifier_code,
  muqd.description
FROM requested_codes requested
JOIN {{SCHEMA}}.measurement_unit_qualifiers muq
  ON muq.measurement_unit_qualifier_code = requested.measurement_unit_qualifier_code
LEFT JOIN {{SCHEMA}}.measurement_unit_qualifier_descriptions muqd
  ON muqd.measurement_unit_qualifier_code = muq.measurement_unit_qualifier_code
ORDER BY muq.measurement_unit_qualifier_code, requested.source_order;
