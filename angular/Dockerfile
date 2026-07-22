FROM node:24-alpine AS builder

WORKDIR /app

COPY package*.json ./

# 若有 package-lock.json，建議使用 npm ci
RUN npm ci

COPY . .

RUN npm run build -- --configuration production

#####################################################
# Runtime Stage
FROM nginx:alpine

COPY --from=builder /app/dist/teams-manager/browser/ /usr/share/nginx/html/