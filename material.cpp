#include "material.h"
#include "Logger.h"

extern Logger logger;

material::material() = default;
material::material(int _type) {

    type = _type;
    raw_materials = std::vector<material>(0);
    switch (type) {
        case 1:
            costPrice = 3000;
            sellPrice = 6000;
            break;
        case 2:
            costPrice = 4400;
            sellPrice = 7600;
            break;
        case 3:
            costPrice = 5800;
            sellPrice = 9200;
            break;
        case 4:
            costPrice = 15400;
            sellPrice = 22500;
            raw_materials.emplace_back(1);
            raw_materials.emplace_back(2);
            break;
        case 5:
            costPrice = 17200;
            sellPrice = 25000;
            raw_materials.emplace_back(1);
            raw_materials.emplace_back(3);
            break;
        case 6:
            costPrice = 19200;
            sellPrice = 27500;
            raw_materials.emplace_back(2);
            raw_materials.emplace_back(3);
            break;
        case 7:
            costPrice = 76000;
            sellPrice = 105000;
            raw_materials.emplace_back(4);
            raw_materials.emplace_back(5);
            raw_materials.emplace_back(6);
            break;
        default:
            logger.write_error("Unknown item.");
    }
    currentPrice = (float)sellPrice;
}
