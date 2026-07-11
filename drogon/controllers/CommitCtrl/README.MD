已完成 syncCommits，並依照你指定的 syncPullRequests 風格實作 GitHub commits 同步到本地資料庫。

修改檔案：
SyncCommits.cc

這次實作包含：

請求參數驗證
必填：repo_id、github_token
選填：since、until、sha（預設 develop）、per_page（1-100）、max_pages（1-50）
依 repo_id 讀取 repositories
先查本地 repositories，取得 owner 與 repo_name
若找不到 repo_id，回傳 404
依登入上下文取得 user_id / user_name
user_id 來源：RequestContext
user_name 先查 users 表，若查不到可用 body 的 user_name 作 fallback
仍無法取得則回傳錯誤
呼叫 GitHub Commits API（分頁）
路徑格式對齊你提供的 fetch 參考
Header 使用：
Accept: application/vnd.github+json
Authorization: Bearer token
X-GitHub-Api-Version: 2022-11-28
per_page/page 分頁抓取，直到資料不足一頁或達 max_pages
將 API 回應映射到 commits 表欄位
commit_sha ← sha
html_url ← html_url（若無則 fallback url）
github_author_id / github_author_login ← 優先 author，次選 committer
message ← commit.message（長度截到 255，避免欄位超長）
committed_at ← commit.author.date（若無則 commit.committer.date）
github_repository_id / github_repository_name ← 本地 repositories
user_id / user_name ← 本地登入使用者
資料庫寫入策略
以 commit_sha + github_repository_id 查是否存在
存在就 UPDATE，不存在就 INSERT
回傳統計：
fetched_count
synced_count
inserted_count
updated_count
skipped_count
錯誤處理
JSON body 錯誤、repo/user 不存在、GitHub API 非 200、JSON parse 失敗、DB 例外都會回傳明確 error JSON 與對應 HTTP status
驗證結果：

我已檢查這個檔案語法錯誤，結果為無錯誤。