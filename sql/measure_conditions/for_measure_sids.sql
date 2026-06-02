WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
)
SELECT
  mc.measure_condition_sid::text,
  mc.measure_sid::text,
  mc.condition_code,
  mc.component_sequence_number::text,
  CASE WHEN mc.condition_duty_amount IS NULL THEN NULL ELSE mc.condition_duty_amount::text END,
  mc.condition_monetary_unit_code,
  mc.condition_measurement_unit_code,
  mc.condition_measurement_unit_qualifier_code,
  mc.certificate_type_code,
  mc.certificate_code,
  mc.action_code,
  mad.description,
  mccd.description,
  ctd.description,
  cd.description,
  mudesc.description,
  muqd.description
FROM {{SCHEMA}}.measure_conditions mc
JOIN requested_sids rs
  ON rs.measure_sid = mc.measure_sid
LEFT JOIN {{SCHEMA}}.measure_action_descriptions mad
  ON mad.action_code = mc.action_code
LEFT JOIN {{SCHEMA}}.measure_condition_code_descriptions mccd
  ON mccd.condition_code = mc.condition_code
LEFT JOIN {{SCHEMA}}.certificate_type_descriptions ctd
  ON ctd.certificate_type_code = mc.certificate_type_code
LEFT JOIN LATERAL (
  SELECT cd.description
  FROM {{SCHEMA}}.certificate_description_periods cdp
  JOIN {{SCHEMA}}.certificate_descriptions cd
    ON cd.certificate_description_period_sid = cdp.certificate_description_period_sid
   AND cd.certificate_type_code = cdp.certificate_type_code
   AND cd.certificate_code = cdp.certificate_code
  WHERE cdp.certificate_type_code = mc.certificate_type_code
    AND cdp.certificate_code = mc.certificate_code
    AND cdp.validity_start_date <= {{ACTUAL_DATE}}
    AND (cdp.validity_end_date IS NULL OR cdp.validity_end_date >= {{ACTUAL_DATE}})
  ORDER BY cdp.validity_start_date DESC
  LIMIT 1
) cd ON true
LEFT JOIN {{SCHEMA}}.measurement_unit_descriptions mudesc
  ON mudesc.measurement_unit_code = mc.condition_measurement_unit_code
LEFT JOIN {{SCHEMA}}.measurement_unit_qualifier_descriptions muqd
  ON muqd.measurement_unit_qualifier_code = mc.condition_measurement_unit_qualifier_code
ORDER BY mc.measure_sid, mc.condition_code, mc.component_sequence_number, mc.measure_condition_sid;
