//
// Created by torment on 2020/4/7.
//

#include <iostream>
#include <cmath>
#include "assets.h"
#include "a_util.h"

using namespace std;

/**
 * --------------------------------------------------------------------------------
 * 目前的操作流程：
 *
 * 生活钱包：
 *   1. 每月记录一笔【零花钱】
 *   2. 每天记录开销
 * 资产钱包：
 *   1. 每月记录一笔【月收入】，区分资产构成【低风险、高风险、基金、股票等】
 *   2. 每天记录资金的涨跌
 *
 * 总资产构成：
 *   1. 生活费：余额宝
 *   2. 低风险投资费：理财、低风险基金
 *   3. 高风险投资费：高风险基金、黄金
 *
 * 目前的资产投资额：
 *   1. 余额宝：15759.19
 *   2. 理财：30480.60
 *   3. 高风险基金：320348.63
 *   4. 低风险基金：320348.63
 *   5. 黄金：5305.30
 *
 * >> 设计目标：投资金额、±总收益、±昨日收益
 *
 * --------------------------------------------------------------------------------
 * asset_manager（资产管理器）：给外层提供的接口，提供添加删除查询等一系列操作。
 * core_controller（控制器）：用来负责资金的转换和各种计算。
 * asset（资产）：单个资产，用来保存当前资产的各种状态
 *
 *
 * --------------------------------------------------------------------------------
 *
 * 按照收入自动按照比例分配
 * 目的：统计每月资产信息，然后自动计算所得收益。
 * 注：功能过于复杂，目前得想想如何利用支付宝实现。
 *
 * 1. 如果灵活控制资金记录？
 * 现在有一个零用钱钱包，但是没有整体资金的记录。
 * 如果分成两个钱包呢？
 * - 生活钱包：用来记录日常的没笔开销
 * - 资产钱包：用来记录总资产
 *
 *   1. 首先拿到工资后，将其拆成 2 个（零用钱和其他）
 *   2. 分别放到【生活钱包】和【资产钱包】
 *
 *   生活钱包：日常消耗的时候进行记录，每个月打入 3000 元。【这些钱要在一个特殊的账号下管理！支付渠道--> 支付宝、微信、银行卡】
 *     要求：灵活可取，收益较低。
 *
 * 2. 按照现在的资金流入可以分为：
 *    资金流入：工资
 *    资金利润：余额宝、定期、基金、黄金、P2P（已被冻结）
 *
 * 3. 如何区分本金和利息？
 *    现在的余额宝中记录的是总数，包括存入的、赚的、赔的，我已经忘了到底投入的多少钱，需要解决这个问题。
 *
 * 资金类型：
 *   - 不可流动：不能拿出来的资金，用来生息
 *     - 基金、股票、黄金：不可取用，如果遇到特殊场景随机应变
 *   - 可流动：
 *     - 债券基金：当紧急缺钱时，才可以使用
 *     - 余额宝：零用钱，可随时取用
 *
 *  流程：
 *    1. 余额宝作为中转站负责所有资金的流转，同时也负责管理零花钱。
 *       旧流程：把钱全部转入余额宝，等定期的时候直接划扣。
 *       新流程：等得到工资，直接开始定投（每月），然后剩余 3000 在余额宝。
 *    2. 基金分为两种，高风险与稳赚
 *
 * 可配置分配比例：
 * - 50% 高风险（6个基金）
 * - 35% 低风险（1个基金，上限20W）
 * - 15% 灵活（上限3W）
 *
 * 六大系数：
 * -
 *
 * 投资类型：
 * > 可自行扩展：目前支持参数，风险、收益、可取程度
 * - 高风险高收益：不可取
 * - 中风险中收益：极端情况下可取
 * - 无风险低收益：随时可取
 *
 */
namespace assets {

    void assets::calculation_begin(double total_asset) {
        total_assets = total_asset;
        pocket_money = round(total_assets * POCKET_MONEY_PROPORTION);
        low_yield_funds = round(total_assets * LOW_YIELD_FUNDS_PROPORTION);
        high_yield_funds = round(total_assets * HIGH_YIELD_PROPORTION);
    }

    void assets::add_total_assets(double total_asset) {
        total_assets += total_asset;
        pocket_money += round(total_asset * POCKET_MONEY_PROPORTION);
        low_yield_funds += round(total_asset * LOW_YIELD_FUNDS_PROPORTION);
        high_yield_funds += round(total_asset * HIGH_YIELD_PROPORTION);
        c_to_string();
    }

    void assets::c_to_string() {
        // 受精卵
        cout << "总资产：\t\t" << total_assets << endl;
        cout << "动态资产：\t" << pocket_money << endl;
        cout << "低收益资产：\t" << low_yield_funds << endl;
        cout << "高收益资产：\t" << high_yield_funds << endl;
        cout << "-" << endl;
    }

    double assets::p_a_in(double a, double interest_rate, int years) {
        double result = pow(1 + interest_rate, years);
        return a * ((result - 1) / (interest_rate * result));
    }

    double assets::f_p_in(double a, double interest_rate, int years) {
        return 0;
    }

    double assets::f_a_in(double a, double interest_rate, int years) {
        double result = pow(1 + interest_rate, years);
        return a * ((result - 1) / (interest_rate));
    }

    double assets::f_a_in_m(double a, double interest_rate, int years) {
        return f_a_in(a / 12, interest_rate / 12, years * 12);
    }
}