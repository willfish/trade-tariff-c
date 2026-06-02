#ifndef TRADE_TARIFF_C_MEASURE_GRAPH_LOADER_H
#define TRADE_TARIFF_C_MEASURE_GRAPH_LOADER_H

#include "domain/commodity_aggregate.h"
#include "request/commodity_request_context.h"

int measure_graph_loader_load_applicable_measures(const CommodityRequestContext *context,
                                                  CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_types(const CommodityRequestContext *context,
                                            CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_components(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate);
int measure_graph_loader_load_resolved_measure_components(const CommodityRequestContext *context,
                                                          CommodityAggregate *aggregate);
int measure_graph_loader_load_measurement_units(const CommodityRequestContext *context,
                                                CommodityAggregate *aggregate);
int measure_graph_loader_load_measurement_unit_qualifiers(const CommodityRequestContext *context,
                                                          CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_conditions(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_condition_permutation_groups(const CommodityRequestContext *context,
                                                                   CommodityAggregate *aggregate);
int measure_graph_loader_load_appendix_5as(const CommodityRequestContext *context,
                                           CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_condition_codes(const CommodityRequestContext *context,
                                                      CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_condition_components(const CommodityRequestContext *context,
                                                           CommodityAggregate *aggregate);
int measure_graph_loader_load_geographical_areas(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate);
int measure_graph_loader_load_excluded_geographical_areas(const CommodityRequestContext *context,
                                                          CommodityAggregate *aggregate);
int measure_graph_loader_load_additional_codes(const CommodityRequestContext *context,
                                               CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_footnotes(const CommodityRequestContext *context,
                                                CommodityAggregate *aggregate);
int measure_graph_loader_load_measure_legal_acts(const CommodityRequestContext *context,
                                                 CommodityAggregate *aggregate);
int measure_graph_loader_load_quota_order_numbers(const CommodityRequestContext *context,
                                                  CommodityAggregate *aggregate);
int measure_graph_loader_load_quota_definitions(const CommodityRequestContext *context,
                                                CommodityAggregate *aggregate);
int measure_graph_loader_load_quota_events(const CommodityRequestContext *context,
                                           CommodityAggregate *aggregate);
int measure_graph_loader_load_quota_closed_and_transferred_events(const CommodityRequestContext *context,
                                                                  CommodityAggregate *aggregate);
int measure_graph_loader_self_test(void);

#endif
