import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';
import { cacheInvalidate } from '../state/cache.js';

const EP = CONFIG.ENDPOINTS.DISPLAY;
const TTL = CONFIG.CACHE_TTL.DISPLAY;

export const displayApi = {
  async get() {
    const res = await api.get(EP, TTL);
    store.set('display', res.data || CONFIG.DEFAULT_DISPLAY);
    return res.data;
  },

  async update(settings) {
    const res = await api.post(EP, settings);
    store.set('display', { ...store.get('display'), ...settings });
    cacheInvalidate(EP);
    return res.data;
  }
};
