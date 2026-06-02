SELECT
  node.goods_nomenclature_sid::text,
  node.goods_nomenclature_item_id,
  node.productline_suffix,
  node.number_indents::text,
  node.position::text,
  node.depth::text,
  to_char(node.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"') AS validity_start_date,
  CASE
    WHEN node.validity_end_date IS NULL THEN NULL
    ELSE to_char(node.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"')
  END AS validity_end_date,
  description.description,
  CASE WHEN NOT EXISTS (
    SELECT 1
    FROM {{SCHEMA}}.goods_nomenclature_tree_nodes child
    WHERE child.position > node.position
      AND child.position < COALESCE((
        SELECT sibling.position
        FROM {{SCHEMA}}.goods_nomenclature_tree_nodes sibling
        WHERE sibling.position > node.position
          AND sibling.depth <= node.depth
          AND sibling.validity_start_date <= {{ACTUAL_DATE}}
          AND (sibling.validity_end_date IS NULL OR sibling.validity_end_date >= {{ACTUAL_DATE}})
        ORDER BY sibling.position
        LIMIT 1
      ), 999999999999::bigint)
      AND child.validity_start_date <= {{ACTUAL_DATE}}
      AND (child.validity_end_date IS NULL OR child.validity_end_date >= {{ACTUAL_DATE}})
  ) THEN '1' ELSE '0' END AS leaf,
  CASE WHEN EXISTS (
    SELECT 1
    FROM {{SCHEMA}}.full_chemicals fc
    WHERE fc.goods_nomenclature_sid = node.goods_nomenclature_sid
  ) THEN '1' ELSE '0' END AS has_chemicals
FROM {{SCHEMA}}.goods_nomenclature_tree_nodes node
JOIN {{SCHEMA}}.goods_nomenclatures gn
  ON gn.goods_nomenclature_sid = node.goods_nomenclature_sid
LEFT JOIN LATERAL (
  SELECT gd.description
  FROM {{SCHEMA}}.goods_nomenclature_description_periods p
  JOIN {{SCHEMA}}.goods_nomenclature_descriptions gd
    ON gd.goods_nomenclature_description_period_sid = p.goods_nomenclature_description_period_sid
   AND gd.goods_nomenclature_sid = p.goods_nomenclature_sid
  WHERE p.goods_nomenclature_sid = node.goods_nomenclature_sid
    AND p.validity_start_date <= {{ACTUAL_DATE}}
    AND (p.validity_end_date IS NULL OR p.validity_end_date >= {{ACTUAL_DATE}})
  ORDER BY p.validity_start_date DESC
  LIMIT 1
) description ON true
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
