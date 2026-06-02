WITH requested_measure_types AS (
  SELECT requested.measure_type_id, requested.source_order
  FROM unnest($1::text[]) WITH ORDINALITY AS requested(measure_type_id, source_order)
  WHERE requested.measure_type_id IS NOT NULL
    AND requested.measure_type_id <> ''
)
SELECT DISTINCT ON (mt.measure_type_id)
  mt.measure_type_id,
  mtd.description,
  mt.measure_type_series_id,
  mtsd.description AS measure_type_series_description,
  mt.measure_component_applicable_code,
  mt.order_number_capture_code,
  mt.trade_movement_code,
  mt.validity_start_date,
  mt.validity_end_date
FROM requested_measure_types requested
JOIN {{SCHEMA}}.measure_types mt
  ON mt.measure_type_id = requested.measure_type_id
LEFT JOIN {{SCHEMA}}.measure_type_descriptions mtd
  ON mtd.measure_type_id = mt.measure_type_id
LEFT JOIN {{SCHEMA}}.measure_type_series_descriptions mtsd
  ON mtsd.measure_type_series_id = mt.measure_type_series_id
ORDER BY mt.measure_type_id, requested.source_order;
