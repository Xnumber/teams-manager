#!/bin/bash

echo "编译 SQLite3 内存数据库示例程序"
echo "================================"

# 基本示例
echo "编译基本示例..."
g++ -std=c++17 sqlite_memory_example.cc -o sqlite_memory_example \
    -ldrogon -lpthread -lsqlite3

if [ $? -eq 0 ]; then
    echo "✓ sqlite_memory_example 编译成功"
else
    echo "✗ sqlite_memory_example 编译失败"
    exit 1
fi

# 高级示例
echo "编译高级示例..."
g++ -std=c++17 sqlite_memory_advanced.cc -o sqlite_memory_advanced \
    -ldrogon -lpthread -lsqlite3

if [ $? -eq 0 ]; then
    echo "✓ sqlite_memory_advanced 编译成功"
else
    echo "✗ sqlite_memory_advanced 编译失败"
    exit 1
fi

echo ""
echo "编译完成！"
echo ""
echo "运行示例："
echo "  ./sqlite_memory_example      - 基本示例"
echo "  ./sqlite_memory_advanced     - 高级示例"
echo ""
