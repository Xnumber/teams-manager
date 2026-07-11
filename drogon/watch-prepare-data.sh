#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/teams-manager"
PID_FILE="/tmp/cpp_dev_pid"

## 清理舊的 entr 監控進程，避免重複監控
if pgrep -f 'entr -d -p sh -c' > /dev/null; then
    echo "🧹 Killing old entr monitors..."
    pkill -f 'entr -d -p sh -c'
fi
# 清理函數
cleanup() {
    if [ -f "$PID_FILE" ]; then
        echo "🛑 Cleaning up PID $(cat "$PID_FILE")..."
        kill $(cat "$PID_FILE") 2>/dev/null
        rm "$PID_FILE"
    fi
    exit 0
}

trap cleanup EXIT INT TERM
# 構建並運行
build_and_run() {   
    # 終止舊進程
    if [ -f "$PID_FILE" ]; then
        echo "🛑 Stopping previous instance..."
        kill $(cat "$PID_FILE") 2>/dev/null
        sleep 1
    fi
    echo "🔨 Building..."
    cmake --build "$BUILD_DIR" || return 1
    
    
    echo "🚀 Starting program..."
    "$EXECUTABLE" &
    echo $! > "$PID_FILE"
    echo "✅ Program started with PID $(cat "$PID_FILE")"
    echo -e "\033[1;36m進行資料測試。\033[0m"
     # 等待服務端口開啟（假設 8080）
    echo "⏳ 等待服務啟動..."
    for i in {1..30}; do
        if nc -z 127.0.0.1 5001; then
            # echo "🌐 服務已啟動，開始測試資料。"
            break
        else
            sleep 1
        fi
    done
    if ! nc -z 127.0.0.1 5001; then
        echo "❌ 服務未啟動，跳過測試資料。"
        return 1
    fi
    bash -c "k6 run k6/prepare-data.ts"
}
# 終止舊進程
if [ -f "$PID_FILE" ]; then
    echo "🛑 Stopping previous instance..."
    kill $(cat "$PID_FILE") 2>/dev/null
    sleep 1
fi



bash "$SCRIPT_DIR/migrators/postgresql/development/setup_db.sh"

if [ "$2" = "rebuild-db" ]; then
    echo "🔄 Rebuilding Database..."
fi

# 處理 rebuild 參數
if [ "$1" = "rebuild" ]; then
    echo "🔄 Rebuilding and restarting..."
    build_and_run
fi



if [ "$1" != "rebuild" ]; then
    # 初始構建
    cmake -B "$BUILD_DIR" -DBUILD_ORM=ON -DUSE_SQLITE3=ON
    # cmake -B "$BUILD_DIR" -DBUILD_ORM=ON -DUSE_SQLITE3=ON
    build_and_run
fi

# 監控文件變化
# find test/data -name "*.sh" | entr -d -p sh -c "bash $0 rebuild-with-db"
# find controllers main.cc -name "*.cpp" -o -name "*.cc" -o -name "*.h" | entr -d -p sh -c "bash $0 rebuild"

# 監控 C++ 檔案
find controllers -name "*.cpp" -o -name "*.cc" | entr -d -p sh -c "bash $0 rebuild" &

# 監控 spec 腳本
# find playwright -name "*.spec.ts" | entr -d -p sh -c "bash $0 rebuild rebuild-db" &

wait




# # 監控所有常見的 C++ 源文件和頭文件
# find . \( \
#     -name "*.cpp" -o \
#     -name "*.cc" -o \
#     -name "*.cxx" -o \
#     -name "*.c++" -o \
#     -name "*.h" -o \
#     -name "*.hpp" -o \
#     -name "*.hxx" -o \
#     -name "*.hh" \
# \) | entr -d -p sh -c "bash $0 rebuild"

# 排除 build 目錄，避免監控生成的文件
# find . -path ./build -prune -o \( \
#     -name "*.cpp" -o \
#     -name "*.cc" -o \
#     -name "*.h" -o \
#     -name "*.hpp" \
# \) -print | entr -d -p sh -c "bash $0 rebuild"

# 最常用的擴展名
# find . -name "*.cpp" -o -name "*.cc" -o -name "*.h" -o -name "*.hpp" | \
#     entr -d -p sh -c "bash $0 rebuild"