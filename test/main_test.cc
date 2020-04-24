//
// Created by torment on 2020/4/2.
// 25岁 0
// 30岁 80W+
// 35岁 200W+
// 40岁 450W+
// 45岁 800W+
// 50岁 1400W+
//

#include <iostream>
#include <include/leveldb/db.h>
#include <test/assets/asset_manager.h>
#include <test/android/include/runtime.h>
#include "main_log.h"
#include "main_test_data.h"
#include "test/assets/assets.h"
#include "test/assets/asset.h"
#include "test/request/include/request.h"
#include "test/request/include/request_queue.h"

using namespace std;
using namespace crequest;

const string TAG = "main_test";

void fund() {
    auto *assets = new assets::assets();

    auto year = 1;
    auto my = 17400;

    // 今年目标：55.75W，最好超多 60W

    double p = my * POCKET_MONEY_PROPORTION;
    double p_ta = p * 12 * year;
    double p_m = assets->f_a_in_m(p * 12, 0.02, year);

    double l = my * LOW_YIELD_FUNDS_PROPORTION;
    double l_ta = l * 12 * year;
    double l_m = assets->f_a_in_m(l * 12, 0.06, year);

    double h = my * HIGH_YIELD_PROPORTION;
    double h_ta = h * 12 * year;
    double h_m = assets->f_a_in_m(h * 12, 0.1, year);

    cout << "-" << endl;
    cout << "[p m]:\t\t" << to_string(p) << endl;
    cout << "[p ta]:\t\t" << to_string(p_ta) << endl;
    cout << "[p f<-a/m]:\t" << to_string(p_m) << endl;

    cout << "-" << endl;
    cout << "[l m]:\t\t" << to_string(l) << endl;
    cout << "[l ta]:\t\t" << to_string(l_ta) << endl;
    cout << "[l f<-a/m]:\t" << to_string(l_m) << endl;

    cout << "-" << endl;
    cout << "[h m]:\t\t" << to_string(h) << endl;
    cout << "[h ta]:\t\t" << to_string(h_ta) << endl;
    cout << "[h f<-a/m]:\t" << to_string(h_m) << endl;

    cout << endl;
    auto p_tt = p_ta + l_ta + h_ta;
    auto h_mt = p_m + l_m + h_m;
    auto p_h = h_mt - p_tt;
    auto p_h_b = to_string(p_h / p_tt * 100) + "%";

    cout << to_string(h_mt) << "  " << to_string(p_tt) << "  " << to_string(p_h) << "  " << p_h_b << endl;
}

void assets_manager() {
    auto am = new assets::asset_manager(); // 通知
    am->add_asset(new assets::asset(17000));
}

void r_android() {
    cout << "r_android" << endl;
    auto *rt = new android::runtime();
    rt->start();

    android::a_app *app = rt->create_application();
//    app->create_activity();
    app->start_app();
}

void r_request() {
    auto queue = request_queue::create();
    queue->add_request(new request());

    request *r = request::get("");
}

int main() {
//    auto *t_data = new TestData();
//
//    leveldb::DB *db;
//    leveldb::Options options;
//
//    // 如果数据库不存在，创建一个
//    options.create_if_missing = true;
//    // "/Users/torment/Workspace/CLionProjects/leveldb/build_db/test_db"
//    leveldb::Status status = leveldb::DB::Open(options, "/tmp/test_db", &db);
//
//    string key2 = t_data->buildRandomTestTitle();
//    string value2 = t_data->getTestData();
//    string key3 = t_data->buildRandomTestTitle();
//    string value3 = t_data->getTestData();
//    string r_value;
//
//    // 写入
//    status = db->Put(leveldb::WriteOptions(), key2, value2);
//    status = db->Put(leveldb::WriteOptions(), key3, value3);
//    // 读取
//    status = db->Get(leveldb::ReadOptions(), key2, &r_value);
//
//    TLogger::log(TAG, key2);
//
//    // 关闭数据库
//    // 清理程序中的数据结构，关闭文件，释放内存，下次打开数据库还在
//    delete db;

//    assets_manager();
    fund();
//    r_android();

    return 1;
}