WITH requested_ids AS (
  SELECT unnest($1::text[]) AS quota_order_number_id
)
SELECT DISTINCT ON (qon.quota_order_number_id)
  qon.quota_order_number_sid::text,
  qon.quota_order_number_id,
  to_char(qon.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN qon.validity_end_date IS NULL THEN NULL ELSE to_char(qon.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END
FROM {{SCHEMA}}.quota_order_numbers qon
JOIN requested_ids requested ON requested.quota_order_number_id = qon.quota_order_number_id
WHERE qon.validity_start_date <= {{ACTUAL_DATE}}
  AND (qon.validity_end_date IS NULL OR qon.validity_end_date >= {{ACTUAL_DATE}})
ORDER BY qon.quota_order_number_id, qon.validity_start_date DESC;
