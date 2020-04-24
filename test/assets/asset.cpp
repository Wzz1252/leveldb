//
// Created by torment on 2020/4/8.
//

#include "asset.h"

namespace assets {

    double asset::get_price() {
        return price;
    }

    std::string asset::to_string() {
        std::string to;
        to.append("liquidity: ").append(std::to_string(liquidity)).append("\n")
                .append("fund: ").append(std::to_string(fund)).append("\n")
                .append("low_fund: ").append(std::to_string(low_fund)).append("\n")
                .append("gold: ").append(std::to_string(gold)).append("\n")
                .append("regular: ").append(std::to_string(regular)).append("\n")
                .append("create_time: ").append(ctime(&create_time)).append("\n");
        return to;
    }
}
