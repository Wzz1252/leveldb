//
// Created by torment on 2020/4/8.
//


#ifndef LEVELDB_ASSET_H
#define LEVELDB_ASSET_H

#include <iostream>
#include <ctime>

// 比例 proportion
#define FUND_PROPORTION 0.5
#define LOW_FUND_PROPORTION 0.35
#define GOLD_PROPORTION 0
#define REGULAR_PROPORTION 0
#define LIQUIDITY_PROPORTION 0.15

namespace assets {

    /**
     * 资产
     */
    class asset {
    public:
        explicit asset(double price) : price(price) {
            liquidity = price * LIQUIDITY_PROPORTION;
            fund = price * FUND_PROPORTION;
            low_fund = price * LOW_FUND_PROPORTION;
            gold = price * GOLD_PROPORTION;
            regular = price * REGULAR_PROPORTION;
            create_time = time(nullptr);
        }

        double get_price();

        std::string to_string();

    private:
        /** 价格 */
        double price;

        /** 流动资金（零花钱） */
        double liquidity;
        /** 基金 */
        double fund;
        /** 低风险基金 */
        double low_fund;
        /** 黄金 */
        double gold;
        /** 定期 */
        double regular;

        time_t create_time;
    };
}

#endif //LEVELDB_ASSET_H
