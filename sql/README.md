# SQL templates

`legacy/commodity_response_legacy.sql` is the archived PostgreSQL JSON renderer from
the pre-typed spike. It is not linked into the binary; the production path uses typed
repositories and `commodity_tree_slice_renderer.c`. Keep the file as a historical
reference until exhaustive parity no longer needs it for comparison.

The template uses explicit placeholders:

- `{{SCHEMA}}` for `uk` or `xi`
- `{{ACTUAL_DATE}}` for `CURRENT_DATE` or a quoted date expression

New typed repository SQL should return rows, not rendered JSON.
