# 快速開始指南

## 方案一：TenantMapper（推薦）

### 步驟 1：複製文件到項目

```bash
# 複製 TenantMapper.h 到你的 utils 目錄
cp tenant_example/solution1_wrapper/TenantMapper.h utils/
```

### 步驟 2：在 Controller 中使用

```cpp
#include "utils/TenantMapper.h"
#include "models/Projects.h"

void MyController::getProjects(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback)
{
    // 從請求中獲取 tenant_id
    auto tenantId = req->attributes()->get<std::string>("tenant_id");
    
    DbClientPtr db = app().getDbClient();
    TenantMapper<Projects> mapper(db, tenantId);
    
    // 自動添加 tenant_id 條件的查詢
    auto projects = mapper.findAll();
    
    // 返回結果...
}
```

### 步驟 3：確保 Filter 設置了 tenant_id

```cpp
// 在你的認證 Filter 中
void AuthFilter::doFilter(const HttpRequestPtr& req, ...)
{
    // 從 JWT 或 session 中提取 tenant_id
    std::string tenantId = extractTenantId(req);
    
    // 設置到請求屬性中
    req->attributes()->insert("tenant_id", tenantId);
    
    fccb();  // 繼續處理請求
}
```

### 完成！

現在所有使用 `TenantMapper` 的查詢都會自動添加 `tenant_id` 條件。

---

## 方案三：TenantRepository（已有 RequestContext）

### 步驟 1：確保 RequestContext 已實現

檢查你的項目中是否已有 `RequestContext` 類（在 `filters/RequestContext/` 目錄）。

### 步驟 2：複製文件

```bash
cp tenant_example/solution3_repository/TenantRepository.h utils/
```

### 步驟 3：修改 TenantRepository.h

找到這一行：
```cpp
class RequestContext
{
public:
    static void setTenantId(const std::string& id);
    static std::string getTenantId();
};
```

改為引用你項目中的 RequestContext：
```cpp
#include "filters/RequestContext/RequestContext.h"
```

### 步驟 4：確保 Filter 中設置了 RequestContext

```cpp
// 在你的 AuthFilter 中
void AuthFilter::doFilter(const HttpRequestPtr& req, ...)
{
    std::string tenantId = extractTenantId(req);
    
    // 設置到 RequestContext
    RequestContext::setTenantId(tenantId);
    
    fccb();
}
```

### 步驟 5：在 Controller 中使用

```cpp
#include "utils/TenantRepository.h"
#include "models/Projects.h"

void MyController::getProjects(const HttpRequestPtr& req,
                               std::function<void(const HttpResponsePtr&)>&& callback)
{
    DbClientPtr db = app().getDbClient();
    
    // 無需傳入 tenantId，自動從 RequestContext 獲取
    TenantRepository<Projects> repo(db);
    
    auto projects = repo.findAll();
    
    // 返回結果...
}
```

---

## 常見問題

### Q: 如何選擇方案？

**A:** 推薦使用方案一（TenantMapper），理由：
- 更明確（需要顯式傳入 tenant_id）
- 易於測試
- 不依賴全局狀態
- 可以在同一請求中操作多個租戶（如果需要）

如果你的項目已經有完善的 RequestContext 機制，並且所有操作都只針對一個租戶，可以選擇方案三。

### Q: 如何測試？

**方案一/二：**
```cpp
TEST_CASE("Test project filtering") {
    DbClientPtr db = getTestDatabase();
    TenantMapper<Projects> mapper(db, "test-tenant-id");
    
    auto projects = mapper.findAll();
    REQUIRE(projects.size() > 0);
    
    // 驗證所有項目都屬於該租戶
    for (const auto& p : projects) {
        REQUIRE(p.getValueOfTenantId() == "test-tenant-id");
    }
}
```

**方案三：**
```cpp
TEST_CASE("Test project filtering with context") {
    RequestContext::setTenantId("test-tenant-id");
    
    DbClientPtr db = getTestDatabase();
    TenantRepository<Projects> repo(db);
    
    auto projects = repo.findAll();
    REQUIRE(projects.size() > 0);
    
    RequestContext::clear();  // 清理
}
```

### Q: 如何從現有代碼遷移？

**步驟 1：** 找到所有使用 `Mapper<Projects>` 的地方

```cpp
// 原代碼
Mapper<Projects> mapper(db);
auto projects = mapper.findAll();
```

**步驟 2：** 替換為 `TenantMapper`

```cpp
// 新代碼
auto tenantId = req->attributes()->get<std::string>("tenant_id");
TenantMapper<Projects> mapper(db, tenantId);
auto projects = mapper.findAll();
```

**步驟 3：** 編譯並測試

### Q: 性能影響？

幾乎沒有性能影響。所有的包裝邏輯都在編譯時確定，運行時只是多了一個 `AND tenant_id = ?` 條件。

### Q: 如何處理不需要 tenant_id 過濾的查詢？

使用原始 Mapper：

```cpp
// 對於管理員查詢所有租戶的數據
Mapper<Projects> mapper(db);
auto allProjects = mapper.findAll();  // 不會過濾 tenant_id
```

或者從 TenantMapper 獲取原始 Mapper：

```cpp
TenantMapper<Projects> tenantMapper(db, tenantId);
auto& rawMapper = tenantMapper.getMapper();
auto allProjects = rawMapper.findAll();
```

---

## 檢查清單

使用前請確保：

- [ ] 已經有認證機制能獲取當前用戶的 tenant_id
- [ ] Filter 或中間件設置了 tenant_id（方案一：設置到 request attributes，方案三：設置到 RequestContext）
- [ ] 數據表中有 tenant_id 欄位
- [ ] Model 已經通過 drogon_ctl 生成（包含 tenant_id）

## 下一步

1. 選擇適合的方案
2. 複製對應的文件到項目中
3. 在一個 Controller 中試用
4. 逐步遷移其他 Controller
5. 編寫測試確保多租戶隔離正確

需要幫助？查看 `comparison.md` 了解詳細對比。
