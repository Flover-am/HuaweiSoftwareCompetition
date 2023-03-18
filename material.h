#ifndef CODECRAFTSDK_MATERIAL_H
#define CODECRAFTSDK_MATERIAL_H


#include <vector>

class material {
public:

    int type{1};
    std::vector<material> raw_materials;
    int costPrice{-1};
    int sellPrice{-1};
    float currentPrice{-1.0f};

    material();
    explicit material(int);
};


#endif //CODECRAFTSDK_MATERIAL_H
