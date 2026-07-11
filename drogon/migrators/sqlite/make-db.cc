#include <thread>
#include <chrono>
#include <drogon/drogon.h>
#include <iostream>
#include "make-db.h"
#include <drogon/orm/DbClient.h>
#include <optional>
using namespace drogon;
using namespace drogon::orm;
using std::optional;

std::optional<drogon::orm::DbClientPtr> makeDb() {
    // 透過設定字串建立 SQLite3 記憶體資料庫客戶端
    // ":memory:" 是 SQLite3 的特殊檔名，表示使用記憶體資料庫
    std::cout << "Generating database..." << std::endl;
    // drogon::app().createDbClient("sqlite3", ":memory:", 1, "", "", "memory");
    // DbClientPtr clientPtr = drogon::app().getDbClient("memory");

    DbClientPtr clientPtr = DbClient::newSqlite3Client("filename=:memory:", 1);
    // DbClientPtr clientPtr = DbClient::newSqlite3Client("filename=:memory:", 1);
    // Sqlite3Config sqliteCfg {
    //     .connectionNumber = 1,
    //     .filename = ":memory:",
    //     .name = "sqlite_memory_db",
    //     .timeout = 3.0,
    // };
    // drogon::app().addDbClient(0);
    // DbClientPtr clientPtr = drogon::app().getDbClient("sqlite_memory_db");
    // 將 DbClientPtr 存儲在 Drogon 的 CustomData 中，以便全局訪問
    // drogon::app().setCustomData<DbClientPtr>(clientPtr);
    // drogon::app().getCustomData<std::shared_ptr<DbClient>().reset(clientPtr.get());
    // drogon::app().addDbClient(
    //     Sqlite3Config {
    //         connectionNumber = 1,
    //         filename = ":memory:",
    //         name = "sqlite_memory_db",
    //         timeout = 3.0
    //     }
    // );
    std::cout << "創建 SQLite3 記憶體數據庫成功！" << std::endl;


    // 创建表结构
    try {
        clientPtr->execSqlSync(
            "CREATE TABLE IF NOT EXISTS users ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    username VARCHAR(50) NOT NULL,"
            "    email VARCHAR(100) NOT NULL,"
            "    age INTEGER,"
            "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ")"
        );
        std::cout << "创建 users 表成功！" << std::endl;
        
        // 插入测试数据
        clientPtr->execSqlSync(
            "INSERT INTO users (username, email, age) VALUES (?, ?, ?)",
            "张三",
            "zhangsan@example.com",
            25
        );
        
        clientPtr->execSqlSync(
            "INSERT INTO users (username, email, age) VALUES (?, ?, ?)",
            "李四",
            "lisi@example.com",
            30
        );
        
        std::cout << "插入测试数据成功！" << std::endl;
        
        // 查询数据
        auto result = clientPtr->execSqlSync("SELECT * FROM users");
        std::cout << "\n查询结果：" << std::endl;
        std::cout << "共 " << result.size() << " 条记录" << std::endl;
        
        for (auto row : result)
        {
            std::cout << "ID: " << row["id"].as<int>() 
                     << ", 姓名: " << row["username"].as<std::string>()
                     << ", 邮箱: " << row["email"].as<std::string>()
                     << ", 年龄: " << row["age"].as<int>()
                     << std::endl;
        }
        
        // 使用事务
        // std::cout << "\n使用事务插入数据..." << std::endl;
        // auto trans = clientPtr->newTransaction();
        // trans->execSqlSync(
        //     "INSERT INTO users (username, email, age) VALUES (?, ?, ?)",
        //     "王五",
        //     "wangwu@example.com",
        //     28
        // );
        // trans->execSqlSync(
        //     "UPDATE users SET age = age + 1 WHERE username = ?",
        //     "张三"
        // );
        // // 提交事务
        // // trans 会在离开作用域时自动提交
        
        // // 再次查询验证
        // result = clientPtr->execSqlSync("SELECT * FROM users ORDER BY id");
        // std::cout << "\n更新后的查询结果：" << std::endl;
        // for (auto row : result)
        // {
        //     std::cout << "ID: " << row["id"].as<int>() 
        //              << ", 姓名: " << row["username"].as<std::string>()
        //              << ", 年龄: " << row["age"].as<int>()
        //              << std::endl;
        // }
        
        // // 演示异步操作
        // std::cout << "\n使用异步方式查询..." << std::endl;
        // clientPtr->execSqlAsync(
        //     "SELECT COUNT(*) as count FROM users",
        //     [](const Result &r) {
        //         std::cout << "用户总数: " << r[0]["count"].as<int>() << std::endl;
        //     },
        //     [](const DrogonDbException &e) {
        //         std::cerr << "查询失败: " << e.base().what() << std::endl;
        //     }
        // );
        
        // 等待异步操作完成
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } catch (const DrogonDbException &e) {
        std::cerr << "資料錯誤: " << e.base().what() << std::endl;
        return std::nullopt;
    }
    return clientPtr;
}