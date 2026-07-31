import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';
import { cacheInvalidate } from '../state/cache.js';

const EP = CONFIG.ENDPOINTS.TIME;
const TTL = CONFIG.CACHE_TTL.STATUS;

export const timeApi = {
  async get() {
    const res = await api.get(EP, TTL);
    store.set('time', res.data || {});
    return res.data;
  },

  async set(timeData) {
    const res = await api.post(EP, timeData);
    cacheInvalidate(EP);
    cacheInvalidate(CONFIG.ENDPOINTS.STATUS);
    return res.data;
  }
};
