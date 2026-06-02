WITH requested_document_codes AS (
  SELECT unnest($1::text[]) AS document_code
)
SELECT
  a5a.certificate_type_code,
  a5a.certificate_code,
  a5a.cds_guidance
FROM {{SCHEMA}}.appendix_5as a5a
JOIN requested_document_codes rdc
  ON rdc.document_code = a5a.certificate_type_code || a5a.certificate_code
ORDER BY a5a.certificate_type_code, a5a.certificate_code;
