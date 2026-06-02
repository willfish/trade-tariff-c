WITH requested_sids AS (
  SELECT unnest($1::text[])::bigint AS quota_definition_sid
),
events AS (
  SELECT qee.quota_definition_sid, qee.occurrence_timestamp, 'exhaustion' AS event_type
  FROM {{SCHEMA}}.quota_exhaustion_events qee
  JOIN requested_sids requested ON requested.quota_definition_sid = qee.quota_definition_sid
  WHERE qee.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp

  UNION ALL

  SELECT qbe.quota_definition_sid, qbe.occurrence_timestamp, 'balance' AS event_type
  FROM {{SCHEMA}}.quota_balance_events qbe
  JOIN requested_sids requested ON requested.quota_definition_sid = qbe.quota_definition_sid
  WHERE qbe.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp

  UNION ALL

  SELECT qce.quota_definition_sid, qce.occurrence_timestamp, 'critical' AS event_type
  FROM {{SCHEMA}}.quota_critical_events qce
  JOIN requested_sids requested ON requested.quota_definition_sid = qce.quota_definition_sid
  WHERE qce.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp

  UNION ALL

  SELECT qre.quota_definition_sid, qre.occurrence_timestamp, 'reopening' AS event_type
  FROM {{SCHEMA}}.quota_reopening_events qre
  JOIN requested_sids requested ON requested.quota_definition_sid = qre.quota_definition_sid
  WHERE qre.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp

  UNION ALL

  SELECT que.quota_definition_sid, que.occurrence_timestamp, 'unblocking' AS event_type
  FROM {{SCHEMA}}.quota_unblocking_events que
  JOIN requested_sids requested ON requested.quota_definition_sid = que.quota_definition_sid
  WHERE que.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp

  UNION ALL

  SELECT qus.quota_definition_sid, qus.occurrence_timestamp, 'unsuspension' AS event_type
  FROM {{SCHEMA}}.quota_unsuspension_events qus
  JOIN requested_sids requested ON requested.quota_definition_sid = qus.quota_definition_sid
  WHERE qus.occurrence_timestamp <= {{ACTUAL_DATE}}::timestamp
)
SELECT DISTINCT ON (quota_definition_sid)
  quota_definition_sid::text,
  to_char(occurrence_timestamp, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  event_type
FROM events
ORDER BY quota_definition_sid, occurrence_timestamp DESC, event_type DESC;
