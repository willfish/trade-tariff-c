#ifndef TRADE_TARIFF_C_GOODS_NOMENCLATURE_DESCRIPTION_H
#define TRADE_TARIFF_C_GOODS_NOMENCLATURE_DESCRIPTION_H

typedef struct {
    const char *description;
    const char *service;
} GoodsNomenclatureDescriptionModel;

GoodsNomenclatureDescriptionModel goods_nomenclature_description_new(const char *description,
                                                                     const char *service);
int goods_nomenclature_description_uk(const GoodsNomenclatureDescriptionModel *model);
int goods_nomenclature_description_self_test(void);

#endif
