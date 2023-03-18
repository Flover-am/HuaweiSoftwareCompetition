//
// Created by 18466 on 2023/3/17.
//

#ifndef CODECRAFTSDK_MATERIAL_H
#define CODECRAFTSDK_MATERIAL_H


#include <vector>

class material {
public:

    int type{1};
    std::vector<material> raw_materials;
    int cost_price{-1};
    int sell_price{-1};
    float current_selling_price{}{-1f};

    material();

    explicit material(int);
};


#endif //CODECRAFTSDK_MATERIAL_H
