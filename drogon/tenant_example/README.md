# Tenant Filter 自動化方案

本目錄包含三種為 Mapper 自動添加 `tenant_id` 篩選條件的實現方案。

## 方案一：包裝類 (Wrapper Pattern) - **推薦**

**資料夾**: `solution1_wrapper/`

### 優點
- ✅ 不需要修改 Drogon 源碼
- ✅ 類型安全，編譯時檢查
- ✅ 易於測試和維護
- ✅ API 清晰，使用者明確知道會自動添加 tenant_id
- ✅ 支持鏈式調用
- ✅ 可選擇性使用，某些場景仍可使用原始 Mapper

### 適用場景
- 需要在多數查詢中自動添加 tenant_id
- 需要靈活切換是否使用租戶過濾
- 團隊開發，需要明確的 API 語義

## 方案二：繼承 Mapper

**資料夾**: `solution2_inheritance/`

### 優點
- 完全兼容 Mapper API
- 可以重寫任意方法

### 缺點
- 繼承模板類較複雜
- 可能與未來 Drogon 版本不兼容

### 適用場景
- 需要完全替換 Mapper 行為
- 不需要原始 Mapper 功能

## 方案三：基於 RequestContext 的 Repository

**資料夾**: `solution3_repository/`

### 優點
- 自動從請求上下文獲取 tenant_id
- 減少參數傳遞
- 適合 DI 架構

### 缺點
- 依賴全局狀態（RequestContext）
- 測試時需要 mock RequestContext

### 適用場景
- 已經使用 RequestContext 存儲租戶信息
- 所有請求都需要租戶隔離
- 使用過濾器統一設置 tenant_id

## 快速比較

| 特性 | 方案一 | 方案二 | 方案三 |
|------|--------|--------|--------|
| 實現複雜度 | 簡單 | 中等 | 簡單 |
| 靈活性 | 高 | 中 | 低 |
| 測試難度 | 易 | 中 | 中 |
| 維護成本 | 低 | 中 | 低 |
| 是否需要修改現有代碼 | 否 | 否 | 需要 RequestContext |

## 推薦使用

**推薦使用方案一**，因為它提供了最佳的靈活性、可維護性和類型安全性。
