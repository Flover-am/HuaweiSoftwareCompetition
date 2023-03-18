//
// Created by 18466 on 2023/3/17.
//

#include "material.h"
#include "Logger.h"

Logger logger(true);

material::material() = default;

material::material(int _type) {
    type = _type;
    raw_materials = std::vector<material>(0);
    switch (type) {
        case 1:
            cost_price = 3000;
            sell_price = 6000;
            break;
        case 2:
            cost_price = 4400;
            sell_price = 7600;
            break;
        case 3:
            cost_price = 5800;
            sell_price = 9200;
            break;
        case 4:
            cost_price = 15400;
            sell_price = 22500;
            raw_materials.emplace_back(1);
            raw_materials.emplace_back(2);
            break;
        case 5:
            cost_price = 17200;
            sell_price = 25000;
            raw_materials.emplace_back(1);
            raw_materials.emplace_back(3);
            break;
        case 6:
            cost_price = 19200;
            sell_price = 27500;
            raw_materials.emplace_back(2);
            raw_materials.emplace_back(3);
            break;
        case 7:
            cost_price = 76000;
            sell_price = 105000;
            raw_materials.emplace_back(4);
            raw_materials.emplace_back(5);
            raw_materials.emplace_back(6);
            break;
        default:
            logger.write_error("未知物品类型被创建");
    }
    current_selling_price = (float) sell_price;

}
