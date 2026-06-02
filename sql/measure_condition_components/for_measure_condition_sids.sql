WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_condition_sid
)
SELECT
  mcc.measure_condition_sid::text,
  mcc.duty_expression_id,
  CASE WHEN mcc.duty_amount IS NULL THEN NULL ELSE mcc.duty_amount::text END,
  mcc.monetary_unit_code,
  mcc.measurement_unit_code,
  mcc.measurement_unit_qualifier_code,
  ded.description
FROM {{SCHEMA}}.measure_condition_components mcc
JOIN requested_sids rs
  ON rs.measure_condition_sid = mcc.measure_condition_sid
LEFT JOIN {{SCHEMA}}.duty_expression_descriptions ded
  ON ded.duty_expression_id = mcc.duty_expression_id
ORDER BY mcc.measure_condition_sid, mcc.duty_expression_id;
