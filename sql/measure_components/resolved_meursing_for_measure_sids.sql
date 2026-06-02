WITH requested_sids AS (
  SELECT unnest($1::integer[]) AS measure_sid
),
root_components AS (
  SELECT
    root.measure_sid AS root_measure_sid,
    root.geographical_area_id AS root_geographical_area_id,
    root.reduction_indicator AS root_reduction_indicator,
    root_component.duty_expression_id AS root_duty_expression_id,
    CASE root_component.duty_expression_id
      WHEN '12' THEN '674'
      WHEN '14' THEN '674'
      WHEN '21' THEN '672'
      WHEN '25' THEN '672'
      WHEN '27' THEN '673'
      WHEN '29' THEN '673'
      ELSE NULL
    END AS meursing_measure_type_id
  FROM {{SCHEMA}}.measures root
  JOIN requested_sids requested
    ON requested.measure_sid = root.measure_sid
  JOIN {{SCHEMA}}.measure_components root_component
    ON root_component.measure_sid = root.measure_sid
  WHERE root_component.duty_expression_id IN ('12', '14', '21', '25', '27', '29')
),
current_geographical_areas AS (
  SELECT
    geographical_area_sid,
    geographical_area_id,
    CASE geographical_area_id
      WHEN 'EU' THEN '1013'
      ELSE geographical_area_id
    END AS referenced_or_self_id
  FROM {{SCHEMA}}.geographical_areas
  WHERE validity_start_date <= {{ACTUAL_DATE}}
    AND (validity_end_date IS NULL OR validity_end_date >= {{ACTUAL_DATE}})
),
expanded_memberships AS (
  SELECT DISTINCT
    group_area.geographical_area_id AS group_geographical_area_id,
    child_area.geographical_area_id AS contained_geographical_area_id
  FROM current_geographical_areas group_area
  JOIN {{SCHEMA}}.geographical_area_memberships membership
    ON membership.geographical_area_group_sid = group_area.geographical_area_sid
   AND membership.validity_start_date <= {{ACTUAL_DATE}}
   AND (membership.validity_end_date IS NULL OR membership.validity_end_date >= {{ACTUAL_DATE}})
  JOIN current_geographical_areas child_area
    ON child_area.geographical_area_sid = membership.geographical_area_sid
),
candidate_measures AS (
  SELECT DISTINCT ON (
    root_components.root_measure_sid,
    root_components.root_duty_expression_id,
    meursing.measure_type_id
  )
    root_components.root_measure_sid,
    meursing.measure_sid AS meursing_measure_sid
  FROM root_components
  JOIN {{SCHEMA}}.measures meursing
    ON meursing.goods_nomenclature_item_id IS NULL
   AND meursing.measure_type_id = root_components.meursing_measure_type_id
   AND meursing.additional_code_type_id = '7'
   AND meursing.additional_code_id = $2
   AND meursing.reduction_indicator IS NOT DISTINCT FROM root_components.root_reduction_indicator
  JOIN {{SCHEMA}}.measure_types measure_type
    ON measure_type.measure_type_id = meursing.measure_type_id
  LEFT JOIN {{SCHEMA}}.base_regulations br
    ON br.base_regulation_id = meursing.measure_generating_regulation_id
   AND br.base_regulation_role = meursing.measure_generating_regulation_role
   AND br.approved_flag IS TRUE
  LEFT JOIN {{SCHEMA}}.modification_regulations mr
    ON mr.modification_regulation_id = meursing.measure_generating_regulation_id
   AND mr.modification_regulation_role = meursing.measure_generating_regulation_role
   AND mr.approved_flag IS TRUE
  LEFT JOIN current_geographical_areas meursing_area
    ON meursing_area.geographical_area_sid = meursing.geographical_area_sid
  WHERE meursing.validity_start_date <= {{ACTUAL_DATE}}
    AND (meursing.validity_end_date IS NULL OR meursing.validity_end_date >= {{ACTUAL_DATE}})
    AND (br.base_regulation_id IS NOT NULL OR mr.modification_regulation_id IS NOT NULL)
    AND coalesce(meursing.validity_start_date, br.validity_start_date, mr.validity_start_date) <= {{ACTUAL_DATE}}
    AND (
      coalesce(
        meursing.validity_end_date,
        br.effective_end_date,
        br.validity_end_date,
        mr.effective_end_date,
        mr.validity_end_date
      ) IS NULL
      OR coalesce(
        meursing.validity_end_date,
        br.effective_end_date,
        br.validity_end_date,
        mr.effective_end_date,
        mr.validity_end_date
      ) >= {{ACTUAL_DATE}}
    )
    AND (
      meursing.geographical_area_id = root_components.root_geographical_area_id
      OR meursing.geographical_area_id = '1011'
      OR EXISTS (
        SELECT 1
        FROM expanded_memberships membership
        WHERE membership.group_geographical_area_id = meursing_area.referenced_or_self_id
          AND membership.contained_geographical_area_id = root_components.root_geographical_area_id
      )
    )
    AND NOT EXISTS (
      SELECT 1
      FROM {{SCHEMA}}.measure_excluded_geographical_areas exclusion
      JOIN current_geographical_areas excluded_area
        ON excluded_area.geographical_area_id = exclusion.excluded_geographical_area
      WHERE exclusion.measure_sid = meursing.measure_sid
        AND (
          excluded_area.referenced_or_self_id = root_components.root_geographical_area_id
          OR EXISTS (
            SELECT 1
            FROM expanded_memberships excluded_membership
            WHERE excluded_membership.group_geographical_area_id = excluded_area.referenced_or_self_id
              AND excluded_membership.contained_geographical_area_id = root_components.root_geographical_area_id
          )
        )
    )
  ORDER BY
    root_components.root_measure_sid,
    root_components.root_duty_expression_id,
    meursing.measure_type_id,
    (meursing.geographical_area_id = root_components.root_geographical_area_id) DESC,
    meursing.measure_sid
)
SELECT
  candidate_measures.meursing_measure_sid::text,
  '04' AS duty_expression_id,
  CASE WHEN component.duty_amount IS NULL THEN NULL ELSE component.duty_amount::text END,
  component.monetary_unit_code,
  component.measurement_unit_code,
  component.measurement_unit_qualifier_code,
  description.description,
  candidate_measures.root_measure_sid::text
FROM candidate_measures
JOIN {{SCHEMA}}.measure_components component
  ON component.measure_sid = candidate_measures.meursing_measure_sid
LEFT JOIN {{SCHEMA}}.duty_expression_descriptions description
  ON description.duty_expression_id = component.duty_expression_id
ORDER BY candidate_measures.root_measure_sid, component.duty_expression_id;
