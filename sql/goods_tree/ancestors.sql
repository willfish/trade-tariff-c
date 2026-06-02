WITH origin AS (
  SELECT
    node.goods_nomenclature_sid,
    node.goods_nomenclature_item_id,
    node.position,
    node.depth
  FROM {{SCHEMA}}.goods_nomenclature_tree_nodes node
  JOIN {{SCHEMA}}.goods_nomenclatures gn
    ON gn.goods_nomenclature_sid = node.goods_nomenclature_sid
  WHERE node.goods_nomenclature_item_id = $1
    AND node.productline_suffix = '80'
    AND node.validity_start_date <= {{ACTUAL_DATE}}
    AND (node.validity_end_date IS NULL OR node.validity_end_date >= {{ACTUAL_DATE}})
    AND gn.validity_start_date <= {{ACTUAL_DATE}}
    AND (gn.validity_end_date IS NULL OR gn.validity_end_date >= {{ACTUAL_DATE}})
    AND NOT EXISTS (
      SELECT 1
      FROM {{SCHEMA}}.hidden_goods_nomenclatures hidden
      WHERE hidden.goods_nomenclature_item_id = node.goods_nomenclature_item_id
    )
  ORDER BY node.goods_nomenclature_sid DESC
  LIMIT 1
)
SELECT
  ancestor.goods_nomenclature_sid::text,
  ancestor.goods_nomenclature_item_id,
  ancestor.productline_suffix,
  ancestor.number_indents::text,
  ancestor.position::text,
  ancestor.depth::text,
  to_char(ancestor.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') AS validity_start_date,
  CASE
    WHEN ancestor.validity_end_date IS NULL THEN NULL
    ELSE to_char(ancestor.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"')
  END AS validity_end_date,
  description.description,
  '0' AS leaf,
  '0' AS has_chemicals
FROM origin
JOIN {{SCHEMA}}.goods_nomenclature_tree_nodes ancestor
  ON ancestor.depth < origin.depth
 AND ancestor.position < origin.position
 AND ancestor.position >= ((origin.position / 10000000000) * 10000000000)
 AND ancestor.position = (
   SELECT max(sibling.position)
   FROM {{SCHEMA}}.goods_nomenclature_tree_nodes sibling
   WHERE sibling.depth = ancestor.depth
     AND sibling.position < origin.position
     AND sibling.position >= ((origin.position / 10000000000) * 10000000000)
     AND sibling.validity_start_date <= {{ACTUAL_DATE}}
     AND (sibling.validity_end_date IS NULL OR sibling.validity_end_date >= {{ACTUAL_DATE}})
 )
 AND ancestor.validity_start_date <= {{ACTUAL_DATE}}
 AND (ancestor.validity_end_date IS NULL OR ancestor.validity_end_date >= {{ACTUAL_DATE}})
JOIN {{SCHEMA}}.goods_nomenclatures gn
  ON gn.goods_nomenclature_sid = ancestor.goods_nomenclature_sid
LEFT JOIN LATERAL (
  SELECT gd.description
  FROM {{SCHEMA}}.goods_nomenclature_description_periods p
  JOIN {{SCHEMA}}.goods_nomenclature_descriptions gd
    ON gd.goods_nomenclature_description_period_sid = p.goods_nomenclature_description_period_sid
   AND gd.goods_nomenclature_sid = p.goods_nomenclature_sid
  WHERE p.goods_nomenclature_sid = ancestor.goods_nomenclature_sid
    AND p.validity_start_date <= {{ACTUAL_DATE}}
    AND (p.validity_end_date IS NULL OR p.validity_end_date >= {{ACTUAL_DATE}})
  ORDER BY p.validity_start_date DESC
  LIMIT 1
) description ON true
WHERE ancestor.goods_nomenclature_item_id NOT LIKE '__00000000'
  AND ancestor.goods_nomenclature_item_id NOT LIKE '____000000'
ORDER BY ancestor.position
