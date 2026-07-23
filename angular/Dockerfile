FROM node:24-alpine AS builder

WORKDIR /app

# 啟用 Corepack 並使用 pnpm
RUN corepack enable

# 先複製 package 檔案，提高快取命中率
COPY package.json pnpm-lock.yaml ./

# 安裝相依套件
# RUN pnpm install --frozen-lockfile
RUN --mount=type=cache,target=/root/.local/share/pnpm/store \
    pnpm install --frozen-lockfile


# 複製專案
COPY . .

# 建置
RUN pnpm run build-production

#####################################################
# Runtime Stage
FROM nginx:alpine

COPY --from=builder /app/dist/teams-manager/browser/ /usr/share/nginx/html/

