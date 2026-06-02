WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS goods_nomenclature_sid
),
deduped AS (
  SELECT DISTINCT ON (
    m.measure_generating_regulation_id,
    m.measure_generating_regulation_role,
    m.measure_type_id,
    m.goods_nomenclature_sid,
    m.geographical_area_id,
    m.geographical_area_sid,
    m.additional_code_type_id,
    m.additional_code_id,
    m.ordernumber
  )
    m.measure_sid,
    m.goods_nomenclature_sid,
    m.goods_nomenclature_item_id,
    m.measure_type_id,
    mtd.description AS measure_type_description,
    m.geographical_area_sid,
    m.geographical_area_id,
    m.additional_code_sid,
    m.additional_code_type_id,
    m.additional_code_id,
    m.ordernumber,
    m.reduction_indicator,
    m.national,
    mt.trade_movement_code,
    mt.measure_type_series_id,
    m.measure_generating_regulation_role,
    m.measure_generating_regulation_id,
    m.validity_start_date,
    m.validity_end_date,
    coalesce(m.validity_start_date, br.validity_start_date, mr.validity_start_date) AS effective_start_date,
    coalesce(
      m.validity_end_date,
      br.effective_end_date,
      br.validity_end_date,
      mr.effective_end_date,
      mr.validity_end_date
    ) AS effective_end_date
  FROM {{SCHEMA}}.measures m
  JOIN requested_sids rs ON rs.goods_nomenclature_sid = m.goods_nomenclature_sid
  JOIN {{SCHEMA}}.measure_types mt ON mt.measure_type_id = m.measure_type_id
  LEFT JOIN {{SCHEMA}}.measure_type_descriptions mtd ON mtd.measure_type_id = m.measure_type_id
  LEFT JOIN {{SCHEMA}}.base_regulations br
    ON br.base_regulation_id = m.measure_generating_regulation_id
   AND br.base_regulation_role = m.measure_generating_regulation_role
   AND br.approved_flag IS TRUE
  LEFT JOIN {{SCHEMA}}.modification_regulations mr
    ON mr.modification_regulation_id = m.measure_generating_regulation_id
   AND mr.modification_regulation_role = m.measure_generating_regulation_role
   AND mr.approved_flag IS TRUE
  WHERE m.validity_start_date <= {{ACTUAL_DATE}}
    AND (m.validity_end_date IS NULL OR m.validity_end_date >= {{ACTUAL_DATE}})
    AND (br.base_regulation_id IS NOT NULL OR mr.modification_regulation_id IS NOT NULL)
    AND coalesce(m.validity_start_date, br.validity_start_date, mr.validity_start_date) <= {{ACTUAL_DATE}}
    AND (
      coalesce(
        m.validity_end_date,
        br.effective_end_date,
        br.validity_end_date,
        mr.effective_end_date,
        mr.validity_end_date
      ) IS NULL
      OR coalesce(
        m.validity_end_date,
        br.effective_end_date,
        br.validity_end_date,
        mr.effective_end_date,
        mr.validity_end_date
      ) >= {{ACTUAL_DATE}}
    )
    AND m.measure_type_id NOT IN ({{EXCLUDED_MEASURE_TYPES}})
  ORDER BY
    m.measure_generating_regulation_id DESC NULLS LAST,
    m.measure_generating_regulation_role DESC NULLS LAST,
    m.measure_type_id DESC NULLS LAST,
    m.goods_nomenclature_sid DESC NULLS LAST,
    m.geographical_area_id DESC NULLS LAST,
    m.geographical_area_sid DESC NULLS LAST,
    m.additional_code_type_id DESC NULLS LAST,
    m.additional_code_id DESC NULLS LAST,
    m.ordernumber DESC NULLS LAST,
    coalesce(m.validity_start_date, br.validity_start_date, mr.validity_start_date) DESC NULLS LAST
)
SELECT
  measure_sid::text,
  goods_nomenclature_sid::text,
  goods_nomenclature_item_id,
  measure_type_id,
  measure_type_description,
  geographical_area_sid::text,
  geographical_area_id,
  additional_code_sid::text,
  additional_code_type_id,
  additional_code_id,
  ordernumber,
  reduction_indicator::text,
  national::text,
  trade_movement_code::text,
  measure_type_series_id,
  measure_generating_regulation_role::text,
  measure_generating_regulation_id,
  to_char(validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN validity_end_date IS NULL THEN NULL ELSE to_char(validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END,
  to_char(effective_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE WHEN effective_end_date IS NULL THEN NULL ELSE to_char(effective_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') END
FROM deduped
ORDER BY
  geographical_area_id ASC NULLS LAST,
  measure_type_id ASC NULLS LAST,
  additional_code_type_id ASC NULLS LAST,
  additional_code_id ASC NULLS LAST,
  ordernumber ASC NULLS LAST,
  effective_end_date ASC NULLS LAST;
