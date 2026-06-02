WITH requested_codes AS (
  SELECT code, ordinality
  FROM unnest($1::text[]) WITH ORDINALITY AS requested(code, ordinality)
)
SELECT
  mcc.condition_code,
  mccd.description,
  mcc.validity_start_date::text,
  CASE WHEN mcc.validity_end_date IS NULL THEN NULL ELSE mcc.validity_end_date::text END
FROM {{SCHEMA}}.measure_condition_codes mcc
JOIN requested_codes rc
  ON rc.code = mcc.condition_code
LEFT JOIN {{SCHEMA}}.measure_condition_code_descriptions mccd
  ON mccd.condition_code = mcc.condition_code
WHERE mcc.validity_start_date <= {{ACTUAL_DATE}}
  AND (mcc.validity_end_date IS NULL OR mcc.validity_end_date >= {{ACTUAL_DATE}})
ORDER BY rc.ordinality, mcc.condition_code;
