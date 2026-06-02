WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
),
requested_measures AS (
  SELECT
    m.measure_sid,
    m.measure_type_id,
    m.measure_generating_regulation_id,
    m.measure_generating_regulation_role
  FROM {{SCHEMA}}.measures m
  JOIN requested_sids rs ON rs.measure_sid = m.measure_sid
  WHERE m.validity_start_date <= {{ACTUAL_DATE}}
    AND (m.validity_end_date IS NULL OR m.validity_end_date >= {{ACTUAL_DATE}})
    AND (m.national IS NULL OR m.national IS FALSE)
),
full_suspensions AS (
  SELECT
    rm.measure_sid,
    rm.measure_type_id,
    fts.full_temporary_stop_regulation_id AS regulation_id,
    fts.full_temporary_stop_regulation_role::text AS role,
    'suspension'::text AS source,
    fts.validity_start_date,
    fts.validity_end_date,
    fts.validity_start_date::text AS effective_start_date,
    fts.effective_enddate::text AS effective_end_date,
    fts.officialjournal_number,
    fts.officialjournal_page::text,
    fts.published_date,
    fts.information_text,
    0 AS sort_order
  FROM requested_measures rm
  JOIN {{SCHEMA}}.fts_regulation_actions action
    ON action.stopped_regulation_id = rm.measure_generating_regulation_id
   AND action.stopped_regulation_role = rm.measure_generating_regulation_role
  JOIN {{SCHEMA}}.full_temporary_stop_regulations fts
    ON fts.full_temporary_stop_regulation_id = action.fts_regulation_id
   AND fts.full_temporary_stop_regulation_role = action.fts_regulation_role
   AND fts.validity_start_date <= {{ACTUAL_DATE}}
   AND (fts.validity_end_date IS NULL OR fts.validity_end_date >= {{ACTUAL_DATE}})
),
partial_suspensions AS (
  SELECT
    rm.measure_sid,
    rm.measure_type_id,
    mpts.partial_temporary_stop_regulation_id AS regulation_id,
    NULL::text AS role,
    'suspension'::text AS source,
    mpts.validity_start_date,
    mpts.validity_end_date,
    mpts.validity_start_date::text AS effective_start_date,
    mpts.validity_end_date::text AS effective_end_date,
    mpts.partial_temporary_stop_regulation_officialjournal_number AS officialjournal_number,
    mpts.partial_temporary_stop_regulation_officialjournal_page::text AS officialjournal_page,
    NULL::date AS published_date,
    NULL::text AS information_text,
    0 AS sort_order
  FROM requested_measures rm
  JOIN {{SCHEMA}}.measure_partial_temporary_stops mpts
    ON mpts.measure_sid = rm.measure_sid
   AND mpts.validity_start_date <= {{ACTUAL_DATE}}
   AND (mpts.validity_end_date IS NULL OR mpts.validity_end_date >= {{ACTUAL_DATE}})
  WHERE NOT EXISTS (
    SELECT 1
    FROM full_suspensions fs
    WHERE fs.measure_sid = rm.measure_sid
  )
),
generating_base AS (
  SELECT
    rm.measure_sid,
    rm.measure_type_id,
    br.base_regulation_id AS regulation_id,
    br.base_regulation_role::text AS role,
    'generating'::text AS source,
    br.validity_start_date,
    br.validity_end_date,
    NULL::text AS effective_start_date,
    NULL::text AS effective_end_date,
    br.officialjournal_number,
    br.officialjournal_page::text,
    br.published_date,
    br.information_text,
    1 AS sort_order
  FROM requested_measures rm
  JOIN {{SCHEMA}}.base_regulations br
    ON br.base_regulation_id = rm.measure_generating_regulation_id
   AND br.base_regulation_role = rm.measure_generating_regulation_role
   AND br.approved_flag IS TRUE
),
generating_modification AS (
  SELECT
    rm.measure_sid,
    rm.measure_type_id,
    mr.modification_regulation_id AS regulation_id,
    mr.modification_regulation_role::text AS role,
    'generating'::text AS source,
    mr.validity_start_date,
    mr.validity_end_date,
    NULL::text AS effective_start_date,
    NULL::text AS effective_end_date,
    mr.officialjournal_number,
    mr.officialjournal_page::text,
    mr.published_date,
    mr.information_text,
    1 AS sort_order
  FROM requested_measures rm
  JOIN {{SCHEMA}}.modification_regulations mr
    ON mr.modification_regulation_id = rm.measure_generating_regulation_id
   AND mr.modification_regulation_role = rm.measure_generating_regulation_role
   AND mr.approved_flag IS TRUE
),
base_behind_modification AS (
  SELECT
    rm.measure_sid,
    rm.measure_type_id,
    br.base_regulation_id AS regulation_id,
    br.base_regulation_role::text AS role,
    'base'::text AS source,
    br.validity_start_date,
    br.validity_end_date,
    NULL::text AS effective_start_date,
    NULL::text AS effective_end_date,
    br.officialjournal_number,
    br.officialjournal_page::text,
    br.published_date,
    br.information_text,
    2 AS sort_order
  FROM requested_measures rm
  JOIN {{SCHEMA}}.modification_regulations mr
    ON mr.modification_regulation_id = rm.measure_generating_regulation_id
   AND mr.modification_regulation_role = rm.measure_generating_regulation_role
   AND mr.approved_flag IS TRUE
  JOIN {{SCHEMA}}.base_regulations br
    ON br.base_regulation_id = mr.base_regulation_id
   AND br.base_regulation_role = mr.base_regulation_role
   AND br.approved_flag IS TRUE
),
legal_acts AS (
  SELECT * FROM full_suspensions
  UNION ALL SELECT * FROM partial_suspensions
  UNION ALL SELECT * FROM generating_base
  UNION ALL SELECT * FROM generating_modification
  UNION ALL SELECT * FROM base_behind_modification
)
SELECT
  measure_sid::text,
  measure_type_id,
  regulation_id,
  role,
  source,
  to_char(validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN validity_end_date IS NULL THEN NULL ELSE to_char(validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END,
  effective_start_date,
  effective_end_date,
  officialjournal_number,
  officialjournal_page,
  CASE WHEN published_date IS NULL THEN NULL ELSE to_char(published_date, 'YYYY-MM-DD') END,
  information_text
FROM legal_acts
ORDER BY measure_sid, sort_order, regulation_id;
