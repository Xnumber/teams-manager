// 工作類別相關測試資料
export const TASK_TYPE_NAME = 'Playwright 測試工作類別 - For Task CRUD';
export const TASK_TYPE_DESC = 'Playwright 測試用 task type';
export const getTaskTypeCreateData = () => ({
    name: TASK_TYPE_NAME,
    description: TASK_TYPE_DESC,
});

// 額外一組工作類別測試資料
export const TASK_TYPE_NAME_2 = 'Playwright 測試工作類別2 - For Task CRUD';
export const TASK_TYPE_DESC_2 = 'Playwright 測試用 task type 2';
export const getTaskTypeCreateData2 = () => ({
    name: TASK_TYPE_NAME_2,
    description: TASK_TYPE_DESC_2,
});
