//
// Created by torment on 2020/4/8.
//

#include "asset_manager.h"

namespace assets {

    int asset_manager::add_asset(asset *asset) {
        assets.push_back(asset);
        return 0;
    }

    std::vector<asset *> asset_manager::get_asset() {
        return assets;
    }

    double asset_manager::get_total_assets() {
        double total_assets = 0;
        for (auto &asset : assets) {
            total_assets += asset->get_price();
        }
        return total_assets;
    }
}