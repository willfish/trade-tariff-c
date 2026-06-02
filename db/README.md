# Duty calculator overlays

These JSON files mirror `trade-tariff-backend/db/` and are read at runtime by `commodity_presenter.c` for duty-calculator metadata (additional-code headings and measurement-unit overlays).

Copy or symlink from the Rails app when updating:

```sh
cp ../hmrc/trade-tariff-backend/db/additional_codes.json .
cp ../hmrc/trade-tariff-backend/db/measurement_units.json .
```

Override paths with `ADDITIONAL_CODES_PATH` or `MEASUREMENT_UNITS_PATH` if needed.