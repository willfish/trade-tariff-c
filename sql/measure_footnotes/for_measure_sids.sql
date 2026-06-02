WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
)
SELECT
  fam.measure_sid::text,
  f.footnote_type_id || f.footnote_id AS code,
  f.footnote_type_id || f.footnote_id AS id,
  fd.description
FROM {{SCHEMA}}.footnote_association_measures fam
JOIN requested_sids rs
  ON rs.measure_sid = fam.measure_sid
JOIN {{SCHEMA}}.footnotes f
  ON f.footnote_type_id = fam.footnote_type_id
 AND f.footnote_id = fam.footnote_id
 AND f.validity_start_date <= {{ACTUAL_DATE}}
 AND (f.validity_end_date IS NULL OR f.validity_end_date >= {{ACTUAL_DATE}})
LEFT JOIN LATERAL (
  SELECT fd.description
  FROM {{SCHEMA}}.footnote_description_periods fdp
  JOIN {{SCHEMA}}.footnote_descriptions fd
    ON fd.footnote_description_period_sid = fdp.footnote_description_period_sid
   AND fd.footnote_type_id = fdp.footnote_type_id
   AND fd.footnote_id = fdp.footnote_id
  WHERE fdp.footnote_type_id = f.footnote_type_id
    AND fdp.footnote_id = f.footnote_id
    AND fdp.validity_start_date <= {{ACTUAL_DATE}}
    AND (fdp.validity_end_date IS NULL OR fdp.validity_end_date >= {{ACTUAL_DATE}})
  ORDER BY fdp.validity_start_date DESC
  LIMIT 1
) fd ON true
ORDER BY fam.measure_sid, f.footnote_type_id NULLS FIRST, f.footnote_id NULLS FIRST;
