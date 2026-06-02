WITH requested_sids AS (
  SELECT requested.goods_nomenclature_sid, requested.source_order
  FROM unnest($1::integer[]) WITH ORDINALITY AS requested(goods_nomenclature_sid, source_order)
)
SELECT
  f.footnote_type_id || f.footnote_id AS code,
  f.footnote_type_id || f.footnote_id AS id,
  fd.description
FROM requested_sids requested
JOIN {{SCHEMA}}.footnote_association_goods_nomenclatures association
  ON association.goods_nomenclature_sid = requested.goods_nomenclature_sid
 AND association.validity_start_date <= {{ACTUAL_DATE}}
 AND (association.validity_end_date IS NULL OR association.validity_end_date >= {{ACTUAL_DATE}})
JOIN {{SCHEMA}}.footnotes f
  ON f.footnote_type_id = association.footnote_type
 AND f.footnote_id = association.footnote_id
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
ORDER BY requested.source_order, f.footnote_type_id NULLS FIRST, f.footnote_id NULLS FIRST;
