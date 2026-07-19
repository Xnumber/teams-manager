---
description: "Use when generating or modifying frontend, backend, or database code in this repository. Enforce stack defaults: Angular + DevExtreme for frontend, C++ Drogon for backend, PostgreSQL for DB work. Keywords: angular, devextreme, drogon, c++, postgresql, sql, api, controller, migration."
name: "Project Stack Defaults"
applyTo:
  "angular/**/*, drogon/**/*"
---
# Project Stack Defaults

- Frontend defaults to Angular with DevExtreme components and patterns.
- Backend defaults to C++ with Drogon conventions and APIs.
- Database defaults to PostgreSQL syntax and capabilities.

## Required Behavior For Code Generation

- Do not propose React, Vue, Next.js, Express, NestJS, Spring, Django, or other non-requested stacks unless explicitly asked.
- Do not propose MySQL- or SQLite-specific SQL by default for new DB work in this repository.
- Keep examples and implementation details aligned with the existing folder layout:
  - `angular/` for UI
  - `drogon/` for backend and API
  - `drogon/liquibase/` for schema changes and migrations

## Frontend Rules (Angular + DevExtreme)

- Use Angular-first patterns and TypeScript for UI implementation.
- Prefer DevExtreme components when a UI widget is needed instead of introducing a different UI kit.
- Keep frontend changes inside the Angular app structure and avoid mixing backend responsibilities into UI code.

## Angular Validation Skill Usage

- After any content change under `angular/**/*`, run the reusable skill command: `/angular-syntax-validation`.
- Use this as the standard post-change validation step before concluding Angular-related tasks.

## Backend Rules (C++ + Drogon)

- Use Drogon controllers, services, filters, and middleware patterns already present in the repository.
- Keep business logic in backend C++ code, not in SQL scripts or frontend code.
- Prefer safe query patterns compatible with PostgreSQL.

## Drogon Validation Skill Usage

- After any content change under `drogon/**/*`, run the reusable skill command: `/drogon-compile-validation`.
- Use this as the standard post-change validation step before concluding Drogon-related tasks.

## Database Rules (PostgreSQL)

- Write SQL compatible with PostgreSQL dialect.
- Place schema changes in Liquibase change sets under `drogon/liquibase/change-sets/`.
- For destructive SQL operations, include a safer phased alternative when possible.

## When To Break These Defaults

- Only break these defaults when the user explicitly requests a different framework, language, or database.
- If the request is ambiguous, ask a short clarification question before switching stack.


