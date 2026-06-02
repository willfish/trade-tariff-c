WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
)
SELECT
  mc.measure_sid::text,
  mc.duty_expression_id,
  CASE WHEN mc.duty_amount IS NULL THEN NULL ELSE mc.duty_amount::text END,
  mc.monetary_unit_code,
  mc.measurement_unit_code,
  mc.measurement_unit_qualifier_code,
  ded.description
FROM {{SCHEMA}}.measure_components mc
JOIN requested_sids rs ON rs.measure_sid = mc.measure_sid
LEFT JOIN {{SCHEMA}}.duty_expression_descriptions ded
  ON ded.duty_expression_id = mc.duty_expression_id
ORDER BY mc.measure_sid, mc.duty_expression_id;
