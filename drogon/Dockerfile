FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    libpq5 \
    libssl3 \
    libjsoncpp25 \
    zlib1g \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY build/teams-manager ./teams-manager
COPY config.json .

RUN useradd -m appuser

USER appuser

CMD ["./teams-manager"]