WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
), direct_areas AS (
  SELECT DISTINCT ga.geographical_area_sid
  FROM {{SCHEMA}}.measures m
  JOIN requested_sids rs
    ON rs.measure_sid = m.measure_sid
  JOIN {{SCHEMA}}.geographical_areas ga
    ON ga.geographical_area_sid = m.geographical_area_sid
), excluded_areas AS (
  SELECT DISTINCT ga.geographical_area_sid
  FROM {{SCHEMA}}.measure_excluded_geographical_areas mega
  JOIN requested_sids rs
    ON rs.measure_sid = mega.measure_sid
  JOIN {{SCHEMA}}.geographical_areas ga
    ON ga.geographical_area_id = mega.excluded_geographical_area
), child_areas AS (
  SELECT DISTINCT child.geographical_area_sid
  FROM direct_areas da
  JOIN {{SCHEMA}}.geographical_area_memberships gam
    ON gam.geographical_area_group_sid = da.geographical_area_sid
   AND gam.validity_start_date <= {{ACTUAL_DATE}}
   AND (gam.validity_end_date IS NULL OR gam.validity_end_date >= {{ACTUAL_DATE}})
  JOIN {{SCHEMA}}.geographical_areas child
    ON child.geographical_area_sid = gam.geographical_area_sid
   AND child.validity_start_date <= {{ACTUAL_DATE}}
   AND (child.validity_end_date IS NULL OR child.validity_end_date >= {{ACTUAL_DATE}})
), requested_areas AS (
  SELECT geographical_area_sid FROM direct_areas
  UNION
  SELECT geographical_area_sid FROM excluded_areas
  UNION
  SELECT geographical_area_sid FROM child_areas
)
SELECT DISTINCT ON (ga.geographical_area_sid)
  ga.geographical_area_sid::text,
  ga.geographical_area_id,
  gad.description,
  ga.parent_geographical_area_group_sid::text
FROM {{SCHEMA}}.geographical_areas ga
JOIN requested_areas ra
  ON ra.geographical_area_sid = ga.geographical_area_sid
LEFT JOIN LATERAL (
  SELECT gad.description
  FROM {{SCHEMA}}.geographical_area_description_periods gadp
  JOIN {{SCHEMA}}.geographical_area_descriptions gad
    ON gad.geographical_area_description_period_sid = gadp.geographical_area_description_period_sid
   AND gad.geographical_area_sid = gadp.geographical_area_sid
  WHERE gadp.geographical_area_sid = ga.geographical_area_sid
    AND gadp.validity_start_date <= {{ACTUAL_DATE}}
    AND (gadp.validity_end_date IS NULL OR gadp.validity_end_date >= {{ACTUAL_DATE}})
  ORDER BY gadp.validity_start_date DESC
  LIMIT 1
) gad ON true
WHERE ga.validity_start_date <= {{ACTUAL_DATE}}
  AND (ga.validity_end_date IS NULL OR ga.validity_end_date >= {{ACTUAL_DATE}})
ORDER BY ga.geographical_area_sid, ga.geographical_area_id;
