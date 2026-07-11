type ApiResponse<T> = {
  result: string; // 'ok' or 'error'
  data?: T; // 成功時的資料
  message?: string; // 錯誤訊息
}