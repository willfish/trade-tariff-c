WITH codes AS (
  SELECT
    substring($1 from 1 for 2) || '00000000' AS chapter_code,
    substring($1 from 1 for 4) || '000000' AS heading_code
),
chapter AS (
  SELECT
    node.goods_nomenclature_sid,
    node.goods_nomenclature_item_id,
    node.validity_start_date,
    node.validity_end_date,
    gd.description,
    note.content AS chapter_note
  FROM codes
  JOIN {{SCHEMA}}.goods_nomenclature_tree_nodes node
    ON node.goods_nomenclature_item_id = codes.chapter_code
   AND node.productline_suffix = '80'
   AND node.validity_start_date <= {{ACTUAL_DATE}}
   AND (node.validity_end_date IS NULL OR node.validity_end_date >= {{ACTUAL_DATE}})
  JOIN {{SCHEMA}}.goods_nomenclatures gn
    ON gn.goods_nomenclature_sid = node.goods_nomenclature_sid
   AND gn.validity_start_date <= {{ACTUAL_DATE}}
   AND (gn.validity_end_date IS NULL OR gn.validity_end_date >= {{ACTUAL_DATE}})
  LEFT JOIN LATERAL (
    SELECT description.description
    FROM {{SCHEMA}}.goods_nomenclature_description_periods period
    JOIN {{SCHEMA}}.goods_nomenclature_descriptions description
      ON description.goods_nomenclature_description_period_sid = period.goods_nomenclature_description_period_sid
     AND description.goods_nomenclature_sid = period.goods_nomenclature_sid
    WHERE period.goods_nomenclature_sid = node.goods_nomenclature_sid
      AND period.validity_start_date <= {{ACTUAL_DATE}}
      AND (period.validity_end_date IS NULL OR period.validity_end_date >= {{ACTUAL_DATE}})
    ORDER BY period.validity_start_date DESC
    LIMIT 1
  ) gd ON true
  LEFT JOIN {{SCHEMA}}.chapter_notes note
    ON note.chapter_id = substring(node.goods_nomenclature_item_id from 1 for 2)
  ORDER BY node.goods_nomenclature_sid DESC
  LIMIT 1
),
heading AS (
  SELECT
    node.goods_nomenclature_sid,
    node.goods_nomenclature_item_id,
    node.validity_start_date,
    node.validity_end_date,
    gd.description
  FROM codes
  JOIN {{SCHEMA}}.goods_nomenclature_tree_nodes node
    ON node.goods_nomenclature_item_id = codes.heading_code
   AND node.productline_suffix = '80'
   AND node.validity_start_date <= {{ACTUAL_DATE}}
   AND (node.validity_end_date IS NULL OR node.validity_end_date >= {{ACTUAL_DATE}})
  JOIN {{SCHEMA}}.goods_nomenclatures gn
    ON gn.goods_nomenclature_sid = node.goods_nomenclature_sid
   AND gn.validity_start_date <= {{ACTUAL_DATE}}
   AND (gn.validity_end_date IS NULL OR gn.validity_end_date >= {{ACTUAL_DATE}})
  LEFT JOIN LATERAL (
    SELECT description.description
    FROM {{SCHEMA}}.goods_nomenclature_description_periods period
    JOIN {{SCHEMA}}.goods_nomenclature_descriptions description
      ON description.goods_nomenclature_description_period_sid = period.goods_nomenclature_description_period_sid
     AND description.goods_nomenclature_sid = period.goods_nomenclature_sid
    WHERE period.goods_nomenclature_sid = node.goods_nomenclature_sid
      AND period.validity_start_date <= {{ACTUAL_DATE}}
      AND (period.validity_end_date IS NULL OR period.validity_end_date >= {{ACTUAL_DATE}})
    ORDER BY period.validity_start_date DESC
    LIMIT 1
  ) gd ON true
  ORDER BY node.goods_nomenclature_sid DESC
  LIMIT 1
),
section AS (
  SELECT
    section.id,
    section.numeral,
    section.title,
    section.position,
    note.content AS section_note
  FROM chapter
  JOIN {{SCHEMA}}.chapters_sections chapter_section
    ON chapter_section.goods_nomenclature_sid = chapter.goods_nomenclature_sid
  JOIN {{SCHEMA}}.sections section
    ON section.id = chapter_section.section_id
  LEFT JOIN {{SCHEMA}}.section_notes note
    ON note.section_id = section.id
  ORDER BY section.position
  LIMIT 1
)
SELECT
  section.id::text,
  section.numeral,
  section.title,
  section.position::text,
  section.section_note,
  chapter.goods_nomenclature_sid::text,
  chapter.goods_nomenclature_item_id,
  chapter.description,
  to_char(chapter.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE
    WHEN chapter.validity_end_date IS NULL THEN NULL
    ELSE to_char(chapter.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"')
  END,
  chapter.chapter_note,
  heading.goods_nomenclature_sid::text,
  heading.goods_nomenclature_item_id,
  heading.description,
  to_char(heading.validity_start_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"'),
  CASE
    WHEN heading.validity_end_date IS NULL THEN NULL
    ELSE to_char(heading.validity_end_date, 'YYYY-MM-DD"T"HH24:MI:SS.MS"Z"')
  END
FROM codes
LEFT JOIN chapter ON true
LEFT JOIN heading ON true
LEFT JOIN section ON true
