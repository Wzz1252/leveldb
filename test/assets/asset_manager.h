//
// Created by torment on 2020/4/8.
//

#ifndef LEVELDB_ASSET_MANAGER_H
#define LEVELDB_ASSET_MANAGER_H

#include "asset.h"
#include <vector>

namespace assets {

    /**
     * 资产管理器，此接口是提供给外层使用的API
     */
    class asset_manager {
    public:
        /** 添加一笔资产 */
        int add_asset(asset *asset);

        /** 获得总资产 */
        std::vector<asset *> get_asset();

        /** 总资产 */
        double get_total_assets();

    private:
        /** 保存整个资产信息 */
        std::vector<asset *> assets;
    };

}
#endif //LEVELDB_ASSET_MANAGER_H
