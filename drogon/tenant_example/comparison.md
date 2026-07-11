# 三種方案詳細比較

## 使用方式對比

### 方案一：TenantMapper（包裝類）

```cpp
// Controller 中使用
auto tenantId = req->attributes()->get<std::string>("tenant_id");
TenantMapper<Projects> mapper(db, tenantId);
auto projects = mapper.findAll();
```

**特點**：
- ✅ 需要明確傳遞 tenant_id（更明確）
- ✅ 易於測試（直接傳入 tenantId）
- ✅ 不依賴全局狀態
- ✅ 可以在同一個請求中操作多個租戶

### 方案二：TenantAwareMapper（繼承）

```cpp
// 使用方式與方案一相同
auto tenantId = req->attributes()->get<std::string>("tenant_id");
TenantAwareMapper<Projects> mapper(db, tenantId);
auto projects = mapper.findAll();
```

**特點**：
- ✅ 完全兼容 Mapper API
- ⚠️ 繼承模板類，可能有版本兼容問題
- ✅ 可以重寫更多方法

### 方案三：TenantRepository（RequestContext）

```cpp
// 更簡潔，不需要傳 tenant_id
TenantRepository<Projects> repo(db);  // 自動從 RequestContext 獲取
auto projects = repo.findAll();
```

**特點**：
- ✅ 最簡潔的使用方式
- ⚠️ 依賴 RequestContext（全局狀態）
- ⚠️ 測試需要 mock RequestContext
- ⚠️ 一個請求只能操作一個租戶

## 完整示例對比

### 場景：獲取租戶的所有項目

#### 方案一：TenantMapper

```cpp
void getProjects(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto tenantId = req->attributes()->get<std::string>("tenant_id");
    DbClientPtr db = app().getDbClient();
    TenantMapper<Projects> mapper(db, tenantId);
    
    auto projects = mapper
        .orderBy("created_at", SortOrder::DESC)
        .limit(10)
        .findAll();
    
    // ... 返回結果
}
```

#### 方案二：TenantAwareMapper

```cpp
void getProjects(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto tenantId = req->attributes()->get<std::string>("tenant_id");
    DbClientPtr db = app().getDbClient();
    TenantAwareMapper<Projects> mapper(db, tenantId);
    
    auto projects = mapper
        .orderBy("created_at", SortOrder::DESC)
        .limit(10)
        .findAll();
    
    // ... 返回結果
}
```

#### 方案三：TenantRepository

```cpp
void getProjects(const HttpRequestPtr& req,
                 std::function<void(const HttpResponsePtr&)>&& callback)
{
    // 不需要獲取 tenantId，已在 Filter 中設置到 RequestContext
    DbClientPtr db = app().getDbClient();
    TenantRepository<Projects> repo(db);
    
    auto projects = repo
        .orderBy("created_at", SortOrder::DESC)
        .limit(10)
        .findAll();
    
    // ... 返回結果
}
```

## 測試難易度對比

### 方案一/二：直接傳入 tenant_id

```cpp
TEST_CASE("Test tenant filtering")
{
    DbClientPtr db = getTestDb();
    TenantMapper<Projects> mapper(db, "test-tenant-123");
    
    auto projects = mapper.findAll();
    // ... 驗證結果
}
```

### 方案三：需要設置 RequestContext

```cpp
TEST_CASE("Test tenant filtering with RequestContext")
{
    // 需要先設置 RequestContext
    RequestContext::setTenantId("test-tenant-123");
    
    DbClientPtr db = getTestDb();
    TenantRepository<Projects> repo(db);
    
    auto projects = repo.findAll();
    
    // 測試後清理
    RequestContext::clear();
}
```

## 性能對比

三種方案的性能基本**完全相同**，因為它們都是在編譯時確定的，運行時開銷幾乎為零。

## 安全性對比

### 跨租戶訪問防護

所有三種方案都提供了相同級別的安全防護：

```cpp
// 所有方案都會自動驗證 tenant_id
auto project = mapper.findById("some-id");
// 如果 some-id 不屬於當前租戶，會拋出異常
```

### 防止誤操作

**方案一最安全**：
```cpp
// 明確知道在操作哪個租戶
TenantMapper<Projects> mapper(db, tenantId);
```

**方案三需要注意**：
```cpp
// 不明確，需要確保 Filter 正確設置了 RequestContext
TenantRepository<Projects> repo(db);
```

## 適用場景推薦

| 場景 | 推薦方案 |
|------|----------|
| 新項目 | 方案一（TenantMapper） |
| 需要明確 API 語義 | 方案一 |
| 已有 RequestContext 架構 | 方案三（TenantRepository） |
| 重度使用多租戶 | 方案三 |
| 需要靈活切換 | 方案一 |
| 單元測試為主 | 方案一/二 |
| 需要操作多個租戶 | 方案一/二 |
| 追求代碼簡潔 | 方案三 |

## 遷移難度

### 從現有 Mapper 遷移

**方案一/二**：
```cpp
// 原代碼
Mapper<Projects> mapper(db);
auto projects = mapper.findAll();

// 遷移後
auto tenantId = req->attributes()->get<std::string>("tenant_id");
TenantMapper<Projects> mapper(db, tenantId);
auto projects = mapper.findAll();
```
**改動**：每個使用 Mapper 的地方都需要傳入 tenantId

**方案三**：
```cpp
// 原代碼
Mapper<Projects> mapper(db);
auto projects = mapper.findAll();

// 遷移後（假設 Filter 已設置 RequestContext）
TenantRepository<Projects> repo(db);
auto projects = repo.findAll();
```
**改動**：替換類名，其他基本不變

## 總結建議

### 🏆 推薦：方案一（TenantMapper）

**理由**：
1. 最明確、最安全
2. 易於測試
3. 不依賴全局狀態
4. 靈活性最高
5. 維護成本最低

### 適合方案三的情況

如果你的項目：
- 已經有完善的 RequestContext 機制
- 所有請求都經過統一的認證 Filter
- 追求代碼簡潔性
- 團隊熟悉 Repository 模式

那麼可以選擇方案三。

### 避免方案二

除非你需要重寫 Mapper 的大量方法，否則不推薦使用繼承方式，因為：
- 繼承模板類複雜
- 可能與未來 Drogon 版本不兼容
- 方案一可以達到相同效果
