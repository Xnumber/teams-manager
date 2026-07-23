podman build -t teams-manager-angular .
podman rmi teams-manager-angular
podman build --no-cache -t teams-manager .

podman run -d \
  --name project-manager-angular \
  -p 8080:80 \
  localhost/teams-manager-angular:latest




podman run -d   --replace   --name project-manager-angular   -p 8080:80   localhost/project-manager-angular:latest



1. 重新 build（不要用舊 cache）
podman build --no-cache -t teams-manager .
2. 確認 image 建立時間
podman images
確認 teams-manager 的 CREATED 時間是剛剛。
3. 刪掉舊 container
如果之前有跑過：

podman ps -a

找到舊 container：

podman rm -f <container_id>

重新建立：

podman run -d --name teams-manager -p 8080:80 teams-manager

另外 Podman 有一個容易踩到的點：如果你用了相同 tag，例如：

podman build -t teams-manager .

但舊 container 還引用舊 image，重建 image 不會自動更新 container。

可以直接清掉舊 image：

podman rmi teams-manager-angular
podman build --no-cache -t teams-manager .







# base image
## podman build -f ./Dockerfile.base -t drogon-base:1.0 .
## podman build --no-cache -f ./Dockerfile.base -t drogon-base:1.0 .

# 建制專案 
## podman build -t teams-manager .

如果你會持續開發 Drogon 專案，最推薦的方法是建立一個自己的基底 Image（base image）。這樣 Drogon 只編譯一次，之後所有專案都能重複使用。

方案一：建立 Drogon Base Image（推薦）
1. 建立 Dockerfile.base
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libpq-dev && \
    rm -rf /var/lib/apt/lists/*

RUN git clone --depth=1 https://github.com/drogonframework/drogon.git /tmp/drogon && \
    cd /tmp/drogon && \
    git submodule update --init && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    cd / && \
    rm -rf /tmp/drogon

建立一次：

podman build -f Dockerfile.base -t drogon-base:1.0 .

可能要花 5~10 分鐘，但只需要一次。

2. 專案 Dockerfile

之後每個專案都只要：

FROM drogon-base:1.0

WORKDIR /app

COPY . .

RUN mkdir build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

EXPOSE 8080

CMD ["./build/myapp"]

以後修改 .cpp：

podman build .

通常只要幾秒到幾十秒。

方案二：Multi-stage（正式部署推薦）

這是很多公司採用的方式。

            Build Image
        ┌─────────────────────┐
        │ Ubuntu + GCC + CMake│
        │ Drogon              │
        └─────────┬───────────┘
                  │ 編譯
                  ▼
             myapp 執行檔
                  │
                  ▼
        ┌─────────────────────┐
        │ Runtime Image       │
        │ Ubuntu 24           │
        │ libssl              │
        │ libpq               │
        │ myapp               │
        └─────────────────────┘

最後 Image 可能：

Build Image：2~3 GB
Runtime Image：150~300 MB
方案三：直接安裝 Drogon 套件（最快）

Ubuntu 24.04 已提供 Drogon 套件，因此不一定要自己從 GitHub 編譯。

直接：

RUN apt-get update && \
    apt-get install -y \
    libdrogon-dev

之後：

find_package(Drogon REQUIRED)

即可編譯。

優點：

建置速度非常快。
Dockerfile 簡潔。
不需要每次編譯 Drogon。

缺點：

套件版本由 Ubuntu 套件庫決定，可能不是最新的 Drogon。
我會建議的配置

如果你的環境是：

Hetzner VPS
Ubuntu 24.04/26 主機
Podman
C++20
Drogon
PostgreSQL

我會建議：

建立 drogon-base:1.0（含編譯工具與 Drogon）。
每個專案都 FROM drogon-base:1.0。
正式部署時，再使用 Multi-stage，只把執行檔和必要的執行期函式庫放進最終映像。

這樣兼顧開發效率（不用重複編譯 Drogon）和部署品質（最終映像較小）。






這個錯誤不是 Drogon 的問題，而是 **Container 裡沒有 CA 憑證**。

```
server certificate verification failed.
CAfile: none
```

你的 Dockerfile 少裝了 `ca-certificates`。

Ubuntu 的最小映像 (`ubuntu:24.04`) 預設通常不包含它，因此 `git clone https://...` 無法驗證 GitHub 的 TLS 憑證。

把 `apt install` 改成：

```dockerfile
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ca-certificates \
    gcc \
    g++ \
    cmake \
    git \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libpq-dev && \
    update-ca-certificates && \
    rm -rf /var/lib/apt/lists/*
```

然後重新 build：

```bash
podman build --no-cache -f Dockerfile.base -t drogon-base:1.0 .
```

---

## 如果還是一樣

先測試是不是 Podman 本身的 CA 問題。

```bash
podman run --rm ubuntu:24.04 bash
```

進去後：

```bash
apt update
apt install -y git ca-certificates

git clone https://github.com/drogonframework/drogon.git
```

如果這裡也失敗，就不是 Dockerfile，而是：

* 公司 Proxy
* 自架 CA
* Podman 的憑證設定
* WSL
* Ubuntu 主機的憑證

---

## 我比較推薦另一種方式（比 git clone 更快）

其實 Drogon 官方有 release tarball，不一定要用 git。

例如：

```dockerfile
RUN apt-get update && \
    apt-get install -y \
    wget \
    ca-certificates
```

然後

```dockerfile
RUN wget https://github.com/drogonframework/drogon/archive/refs/tags/v1.9.10.tar.gz
```

或直接使用 Ubuntu 套件：

```dockerfile
RUN apt-get install -y libdrogon-dev
```

這樣連 Git 都不用裝，建置速度會更快。

---

**想確認一件事：你的 Podman 是安裝在**

1. **Ubuntu 實體主機**
2. **WSL2**
3. **macOS Podman Machine**
4. **Windows Podman Desktop**

不同環境造成這個憑證錯誤的原因和解法會有些差異。


# 小tip
你的主機上有：

nlohmann-json3-dev/noble 3.11.3-1

所以你的 container base image 也加入：

nlohmann-json3-dev

即可。

例如：

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    libjsoncpp-dev \
    nlohmann-json3-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    libpq-dev \
    libargon2-dev \
    libcpp-jwt-dev && \
    rm -rf /var/lib/apt/lists/*

重新建立：

podman build --no-cache \
    -f Dockerfile.base \
    -t drogon-base:1.0 .

補充一下，你現在的 base image 依賴已經慢慢成形：

drogon-base
├── build-essential
├── cmake
├── Drogon
├── PostgreSQL client
├── Jsoncpp
├── UUID
├── OpenSSL
├── Zlib
├── Argon2
├── jwt (libcpp-jwt-dev)
└── nlohmann-json

這種方式比每個專案 Dockerfile 自己裝套件好維護。

如果想讓版本完全一致，也可以把主機目前的套件清單匯出：

apt list --installed | grep -E "drogon|jwt|json|argon|pq|ssl|uuid|zlib"

整理後放進 Dockerfile.base，讓 Hetzner 主機和 container 的開發環境一致。