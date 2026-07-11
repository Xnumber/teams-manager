// #include <drogon/drogon.h>
// using namespace drogon;



// class TenantDbMiddleware :
//     public drogon::HttpMiddleware<TenantDbMiddleware>
// {
// public:
//     void invoke(const drogon::HttpRequestPtr &req,
//                 MiddlewareNextCallback &&next,
//                 MiddlewareCallback &&mcb) override
//     {
//         auto tenantId =
//             req->attributes()->get<std::string>("tenant_id");

//         auto client = drogon::app().getDbClient();

//         client->newTransaction(
//             [req,
//              tenantId,
//              next = std::move(next),
//              mcb = std::move(mcb)]
//             (const drogon::orm::TransactionPtr &tx) mutable
//         {
//             // 把 transaction 放進 request
//             req->attributes()->insert("tx", tx);

//             // 設定 tenant
//             tx->execSqlAsync(
//                 "SET LOCAL app.tenant_id = $1",
//                 [req, tx, next = std::move(next), mcb = std::move(mcb)]
//                 (const drogon::orm::Result &) mutable
//                 {
//                     next(req,
//                         [tx, mcb = std::move(mcb)]
//                         (const drogon::HttpResponsePtr &resp) mutable
//                     {
//                         // controller 完成後 commit
//                         tx->commit();

//                         mcb(resp);
//                     });
//                 },
//                 [](const drogon::orm::DrogonDbException &e)
//                 {
//                     LOG_ERROR << e.base().what();
//                 },
//                 tenantId);
//         });
//     }
// };