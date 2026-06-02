WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS additional_code_sid
)
SELECT DISTINCT ON (ac.additional_code_sid)
  ac.additional_code_sid::text,
  ac.additional_code_type_id,
  ac.additional_code,
  acd.description
FROM {{SCHEMA}}.additional_codes ac
JOIN requested_sids rs
  ON rs.additional_code_sid = ac.additional_code_sid
LEFT JOIN LATERAL (
  SELECT acd.description
  FROM {{SCHEMA}}.additional_code_description_periods acdp
  JOIN {{SCHEMA}}.additional_code_descriptions acd
    ON acd.additional_code_description_period_sid = acdp.additional_code_description_period_sid
   AND acd.additional_code_sid = acdp.additional_code_sid
  WHERE acdp.additional_code_sid = ac.additional_code_sid
    AND acdp.validity_start_date <= {{ACTUAL_DATE}}
    AND (acdp.validity_end_date IS NULL OR acdp.validity_end_date >= {{ACTUAL_DATE}})
  ORDER BY acdp.validity_start_date DESC
  LIMIT 1
) acd ON true
WHERE ac.validity_start_date <= {{ACTUAL_DATE}}
  AND (ac.validity_end_date IS NULL OR ac.validity_end_date >= {{ACTUAL_DATE}})
ORDER BY ac.additional_code_sid, ac.additional_code_type_id, ac.additional_code;
