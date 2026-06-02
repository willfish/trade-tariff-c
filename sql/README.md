# SQL templates

`commodity_response_legacy.sql` is the preserved legacy commodity renderer SQL.
It still returns the nearly complete JSON:API document and exists as a
behaviour-preserving boundary while typed repositories and yyjson renderers
replace response slices incrementally.

The template uses explicit placeholders:

- `{{SCHEMA}}` for `uk` or `xi`
- `{{ACTUAL_DATE}}` for `CURRENT_DATE` or a quoted date expression

New typed repository SQL should return rows, not rendered JSON.
