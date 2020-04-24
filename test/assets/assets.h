//
// Created by torment on 2020/4/7.
//

#ifndef LEVELDB_ASSETS_H
#define LEVELDB_ASSETS_H

/** 零花钱比率 */
#define POCKET_MONEY_PROPORTION 0.15
/** 低收益资金比率 */
#define LOW_YIELD_FUNDS_PROPORTION 0.20
/** 高收益资金比率 管理 */
#define HIGH_YIELD_PROPORTION 0.65

namespace assets {

    /**
     * 资产
     */
    class assets {
    public:
        /** 总资产 */
        double total_assets = 0;
        /** 零花钱 */
        double pocket_money = 0;
        /** 低收益资金 */
        double low_yield_funds = 0;
        /** 高收益资金 */
        double high_yield_funds = 0;

        /** 计算开始 */
        void calculation_begin(double total_asset);

        /** 添加一次总资产 */
        void add_total_assets(double total_asset);

        /**
         * 年金现值系数：根据年金求现值
         *
         * @param a 金额
         * @param interest_rate 贴现率（非百分比）
         * @param years 年数
         * @return
         */
        double p_a_in(double a, double interest_rate, int years);

        /**
         * 根据现值求年金
         *
         * @param a 金额
         * @param interest_rate 贴现率（非百分比）
         * @param years 年数
         * @return
         */
        double a_p_in(double a, double interest_rate, int years);

        /**
         * 根据现值求终止
         * @param a 金额
         * @param interest_rate 贴现率（非百分比）
         * @param years 年数
         * @return
         */
        double f_p_in(double a, double interest_rate, int years);

        /**
         * 年金终值系数：根据年金求终值
         *
         * @param a 金额
         * @param interest_rate 贴现率（非百分比）
         * @param years 年数
         * @return
         */
        double f_a_in(double a, double interest_rate, int years);
        // 系数

        /**
         * 年金终值系数：根据年金求终值
         *
         * > 以月为单位
         *
         * @param a 金额
         * @param interest_rate 贴现率（非百分比）
         * @param years 年数
         * @return
         */
        double f_a_in_m(double a, double interest_rate, int month);

        void c_to_string();
    };

}
#endif //LEVELDB_ASSETS_H
