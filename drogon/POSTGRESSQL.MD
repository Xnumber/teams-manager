# 建立資料庫
sudo service postgresql status

sudo service postgresql start


sudo -i -u postgres


## 方法一

createdb testdb


## 方法二

psql

CREATE DATABASE mydatabase;

### 查看資料庫列表
\l
### 離開
\q

### 如果想指定使用者建立
createdb mydatabase -O myuser

### migration - ai
The Drogon C++ web framework has built-in support for database migrations through its command-line interface, drogon_ctl. This feature allows developers to manage database schema changes using C++ code, similar to other web frameworks like Django or Laravel. 
Key Concepts
Migration Files: These are C++ files that contain up() and down() methods. The up() method is for applying the schema changes (e.g., creating a table), and the down() method is for rolling them back (e.g., dropping a table).
drogon_ctl: The command-line tool used to create and apply migrations.
schema_version Table: Drogon automatically creates this table in your database to track which migrations have already been applied, preventing the same migration from running multiple times. 
How to Use Drogon Migrations
Here is a step-by-step guide on how to create and run a migration in Drogon:
Create a New Migration:
Use the drogon_ctl create migration command to generate a new migration file. You can optionally provide a name and column definitions.
bash
drogon_ctl create migration create_users_table name:string email:string is_admin:bool
This command creates a new C++ file with a timestamp in the name (e.g., 20250523153000_create_users_table.cpp).
Review the Generated File:
The generated file will contain a class derived from Migration with pre-filled up() and down() methods, using a TableBuilder to define the schema in a portable way.
cpp
#include <drogon/Migration.h>

class CreateUsersTableMigration : public drogon::orm::Migration {
public:
    void up() override {
        drogon::orm::internal::createTable("users", [this](drogon::orm::TableBuilder& table) {
            table.primaryKey("id");
            table.string("name");
            table.string("email");
            table.boolean("is_admin");
        });
    }

    void down() override {
        drogon::orm::internal::dropTable("users");
    }
};
Apply the Migration:
Use the drogon_ctl db:migrate command to run all pending migrations against your configured database.
bash
drogon_ctl db:migrate
This command connects to the database specified in your configuration (e.g., config.json), checks the schema_version table, and runs any new up() methods.
Rollback the Last Migration (Optional):
You can revert the most recent migration using the drogon_ctl db:rollback command, which executes the down() method.
bash
drogon_ctl db:rollback