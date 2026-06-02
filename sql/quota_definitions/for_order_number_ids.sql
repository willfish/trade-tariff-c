WITH requested_ids AS (
  SELECT unnest($1::text[]) AS quota_order_number_id
),
definitions AS (
  SELECT DISTINCT ON (qd.quota_order_number_id)
    qd.*
  FROM {{SCHEMA}}.quota_definitions qd
  JOIN requested_ids requested ON requested.quota_order_number_id = qd.quota_order_number_id
  WHERE qd.validity_start_date <= {{ACTUAL_DATE}}
    AND (qd.validity_end_date IS NULL OR qd.validity_end_date >= {{ACTUAL_DATE}})
    AND qd.quota_order_number_id NOT LIKE '094%'
    AND qd.quota_order_number_id NOT LIKE '084%'
    AND qd.quota_order_number_id NOT LIKE '074%'
    AND qd.quota_order_number_id NOT LIKE '064%'
    AND qd.quota_order_number_id NOT LIKE '054%'
    AND qd.quota_order_number_id NOT LIKE '044%'
    AND qd.quota_order_number_id NOT LIKE '034%'
    AND qd.quota_order_number_id NOT LIKE '024%'
    AND qd.quota_order_number_id NOT LIKE '014%'
  ORDER BY qd.quota_order_number_id, qd.validity_start_date DESC
),
latest_balance_events AS (
  SELECT DISTINCT ON (qbe.quota_definition_sid)
    qbe.quota_definition_sid,
    qbe.new_balance,
    qbe.last_import_date_in_allocation,
    qbe.occurrence_timestamp
  FROM {{SCHEMA}}.quota_balance_events qbe
  JOIN definitions qd ON qd.quota_definition_sid = qbe.quota_definition_sid
  WHERE qbe.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp
  ORDER BY qbe.quota_definition_sid, qbe.occurrence_timestamp DESC
),
latest_suspension_periods AS (
  SELECT DISTINCT ON (qsp.quota_definition_sid)
    qsp.quota_definition_sid,
    qsp.suspension_start_date,
    qsp.suspension_end_date
  FROM {{SCHEMA}}.quota_suspension_periods qsp
  JOIN definitions qd ON qd.quota_definition_sid = qsp.quota_definition_sid
  WHERE qsp.suspension_start_date <= {{ACTUAL_DATE}}
    AND qsp.suspension_end_date >= {{ACTUAL_DATE}}
  ORDER BY qsp.quota_definition_sid, qsp.suspension_start_date DESC
),
latest_blocking_periods AS (
  SELECT DISTINCT ON (qbp.quota_definition_sid)
    qbp.quota_definition_sid,
    qbp.blocking_start_date,
    qbp.blocking_end_date
  FROM {{SCHEMA}}.quota_blocking_periods qbp
  JOIN definitions qd ON qd.quota_definition_sid = qbp.quota_definition_sid
  WHERE qbp.blocking_start_date <= {{ACTUAL_DATE}}
    AND qbp.blocking_end_date >= {{ACTUAL_DATE}}
  ORDER BY qbp.quota_definition_sid, qbp.blocking_start_date DESC
),
exhausted_definitions AS (
  SELECT DISTINCT qee.quota_definition_sid
  FROM {{SCHEMA}}.quota_exhaustion_events qee
  JOIN definitions qd ON qd.quota_definition_sid = qee.quota_definition_sid
  WHERE qee.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp
),
latest_critical_events AS (
  SELECT DISTINCT ON (qce.quota_definition_sid)
    qce.quota_definition_sid,
    qce.critical_state
  FROM {{SCHEMA}}.quota_critical_events qce
  JOIN definitions qd ON qd.quota_definition_sid = qce.quota_definition_sid
  WHERE qce.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp
  ORDER BY qce.quota_definition_sid, qce.occurrence_timestamp DESC
)
SELECT
  qd.quota_definition_sid::text,
  qd.quota_order_number_id,
  qd.quota_order_number_sid::text,
  qd.initial_volume::text,
  COALESCE(lbe.new_balance, qd.initial_volume)::text,
  qd.description,
  qd.measurement_unit_code,
  qd.monetary_unit_code,
  qd.measurement_unit_qualifier_code,
  CASE WHEN lbe.last_import_date_in_allocation IS NULL THEN NULL ELSE to_char(lbe.last_import_date_in_allocation, 'YYYY-MM-DD"T"00:00:00.000"Z"') END,
  suspension.suspension_start_date::text,
  suspension.suspension_end_date::text,
  blocking.blocking_start_date::text,
  blocking.blocking_end_date::text,
  to_char(qd.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN qd.validity_end_date IS NULL THEN NULL ELSE to_char(qd.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END,
  mud.description,
  mua.abbreviation,
  qd.critical_state,
  critical.critical_state,
  exhausted.quota_definition_sid::text
FROM definitions qd
LEFT JOIN latest_balance_events lbe ON lbe.quota_definition_sid = qd.quota_definition_sid
LEFT JOIN latest_suspension_periods suspension ON suspension.quota_definition_sid = qd.quota_definition_sid
LEFT JOIN latest_blocking_periods blocking ON blocking.quota_definition_sid = qd.quota_definition_sid
LEFT JOIN exhausted_definitions exhausted ON exhausted.quota_definition_sid = qd.quota_definition_sid
LEFT JOIN latest_critical_events critical ON critical.quota_definition_sid = qd.quota_definition_sid
LEFT JOIN {{SCHEMA}}.measurement_unit_descriptions mud ON mud.measurement_unit_code = qd.measurement_unit_code
LEFT JOIN {{SCHEMA}}.measurement_unit_abbreviations mua
  ON mua.measurement_unit_code = qd.measurement_unit_code
 AND COALESCE(mua.measurement_unit_qualifier, '') = COALESCE(qd.measurement_unit_qualifier_code, '')
ORDER BY qd.quota_order_number_id;
