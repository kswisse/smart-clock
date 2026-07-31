import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.DEVICE;
const TTL = CONFIG.CACHE_TTL.DEVICE;

export const deviceApi = {
  async get() {
    const res = await api.get(EP, TTL);
    store.set('device', res.data || {});
    return res.data;
  }
};
