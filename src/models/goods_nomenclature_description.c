#include "models/goods_nomenclature_description.h"

#include <string.h>

GoodsNomenclatureDescriptionModel goods_nomenclature_description_new(const char *description,
                                                                     const char *service)
{
    return (GoodsNomenclatureDescriptionModel){ description, service };
}

int goods_nomenclature_description_uk(const GoodsNomenclatureDescriptionModel *model)
{
    return model == NULL || model->service == NULL || strcmp(model->service, "xi") != 0;
}

int goods_nomenclature_description_self_test(void)
{
    GoodsNomenclatureDescriptionModel uk = goods_nomenclature_description_new("x", "uk");
    GoodsNomenclatureDescriptionModel xi = goods_nomenclature_description_new("x", "xi");
    return goods_nomenclature_description_uk(&uk) &&
           !goods_nomenclature_description_uk(&xi);
}
