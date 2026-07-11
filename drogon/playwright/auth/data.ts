

// 註冊租戶測試資料
export const tenantBaseData = {
	name: 'RD',
	description: 'API-TEST-DESC',
};

export const tenantMissingData = {
	name: 'API-TEST-TENANT-MISSING',
	description: 'API-TEST-DESC-MISSING',
};

// 註冊使用者測試資料
export const userDataForRegister = {
	tenant_name: tenantBaseData.name,
	username: 'user',
	email: `API-TEST-USER-${Date.now()}@example.com`,
	password: '123123',
};

export const registerMissingFieldsData = {
	tenant_name: '測試組織',
	username: '',
	email: '',
	password: '',
};
