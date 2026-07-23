# FROM ubuntu:24.04

# ENV DEBIAN_FRONTEND=noninteractive

# RUN apt-get update && apt-get install -y --no-install-recommends \
#     gcc \
#     g++ \
#     cmake \
#     git \
#     libjsoncpp-dev \
#     uuid-dev \
#     zlib1g-dev \
#     openssl \
#     libssl-dev \
#     libpq-dev &&\
#     rm -rf /var/lib/apt/lists/*
#     # postgresql-all

# # 安裝 Drogon
# RUN git clone https://github.com/drogonframework/drogon && \
#     cd drogon && \
#     git submodule update --init && \
#     mkdir build && \
#     cd build && \
#     cmake .. && \
#     make -j$(nproc) && \
#     make install

# WORKDIR /app

# COPY . .

# RUN mkdir build && \
#     cd build && \
#     cmake .. && \
#     make -j$(nproc)

# EXPOSE 8080

# CMD ["./build/teams-manager"]











FROM drogon-base:1.0

WORKDIR /app

COPY . .

RUN mkdir build && \
cd build && \
cmake .. && \
make -j$(nproc)
COPY ./config.json ./build/config.json

EXPOSE 5001

CMD ["./build/teams-manager"]