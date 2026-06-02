WITH requested_sids AS (
  SELECT requested.goods_nomenclature_sid, requested.source_order
  FROM unnest($1::integer[]) WITH ORDINALITY AS requested(goods_nomenclature_sid, source_order)
)
SELECT DISTINCT ON (guide.id)
  guide.id::text,
  guide.title,
  guide.url
FROM requested_sids requested
JOIN {{SCHEMA}}.guides_goods_nomenclatures association
  ON association.goods_nomenclature_sid = requested.goods_nomenclature_sid
JOIN {{SCHEMA}}.guides guide
  ON guide.id = association.guide_id
ORDER BY guide.id, requested.source_order, association.id;
