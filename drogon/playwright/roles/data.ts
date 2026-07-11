import { host } from '../const/enviroment';

export const ROLE_BASE_URL = `${host}/roles`;

export const ROLE_SINGLE_URL = (id: string) => `${host}/roles/${id}`;

export const roleCreateData = {
  name: 'Testing Role',
  description: '測試的腳色',
};

export const ROLE_NAME_UPDATE = 'Testing Role Updated';

export const ROLE_DESCRIPTION_UPDATE = '更新後測試的腳色描述';