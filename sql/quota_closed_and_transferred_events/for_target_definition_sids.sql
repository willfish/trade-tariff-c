WITH requested_sids AS (
  SELECT unnest($1::text[])::bigint AS quota_definition_sid
),
events AS (
  SELECT DISTINCT ON (qcte.target_quota_definition_sid)
    qcte.*
  FROM {{SCHEMA}}.quota_closed_and_transferred_events qcte
  JOIN requested_sids requested ON requested.quota_definition_sid = qcte.target_quota_definition_sid
  WHERE qcte.closing_date < {{ACTUAL_DATE}}
  ORDER BY qcte.target_quota_definition_sid, qcte.closing_date DESC, qcte.occurrence_timestamp DESC
)
SELECT
  event.quota_definition_sid::text,
  to_char(event.occurrence_timestamp, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  event.target_quota_definition_sid::text,
  event.closing_date::text,
  event.transferred_amount::text,
  to_char(source_definition.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN source_definition.validity_end_date IS NULL THEN NULL ELSE to_char(source_definition.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END,
  source_mud.description,
  source_mua.abbreviation,
  to_char(target_definition.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN target_definition.validity_end_date IS NULL THEN NULL ELSE to_char(target_definition.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END,
  target_mud.description,
  target_mua.abbreviation
FROM events event
LEFT JOIN {{SCHEMA}}.quota_definitions source_definition
  ON source_definition.quota_definition_sid = event.quota_definition_sid
LEFT JOIN {{SCHEMA}}.measurement_unit_descriptions source_mud
  ON source_mud.measurement_unit_code = source_definition.measurement_unit_code
LEFT JOIN {{SCHEMA}}.measurement_unit_abbreviations source_mua
  ON source_mua.measurement_unit_code = source_definition.measurement_unit_code
 AND COALESCE(source_mua.measurement_unit_qualifier, '') = COALESCE(source_definition.measurement_unit_qualifier_code, '')
LEFT JOIN {{SCHEMA}}.quota_definitions target_definition
  ON target_definition.quota_definition_sid = event.target_quota_definition_sid
LEFT JOIN {{SCHEMA}}.measurement_unit_descriptions target_mud
  ON target_mud.measurement_unit_code = target_definition.measurement_unit_code
LEFT JOIN {{SCHEMA}}.measurement_unit_abbreviations target_mua
  ON target_mua.measurement_unit_code = target_definition.measurement_unit_code
 AND COALESCE(target_mua.measurement_unit_qualifier, '') = COALESCE(target_definition.measurement_unit_qualifier_code, '')
ORDER BY event.target_quota_definition_sid;
