// 使用者相關測試資料
export const PLAYWRIGHTTESTUSER = 'abc';
const email_send = `playwright_creator_${Date.now()}@example.com`;
const password_send = 'abcabc';

export const PLAYWRIGHTTESTUSER2 = 'PlaywrightTestUser2';
const email_send2 = `playwright_creator2_${Date.now()}@example.com`;
const password_send2 = 'playwright_pw2';

export const getUserCreateData = (tenant_id: string) => ({
    tenant_id: tenant_id,
    username: PLAYWRIGHTTESTUSER,
    email: email_send,
    password: password_send,
});

export const getUserCreateData2 = (tenantName: string) => ({
    // tenant_id: tenant_id,
    tenantName: tenantName,
    username: PLAYWRIGHTTESTUSER2,
    email: email_send2,
    password: password_send2,
});
