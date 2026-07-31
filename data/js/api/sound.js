import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';
import { cacheInvalidate } from '../state/cache.js';

const EP = CONFIG.ENDPOINTS.SOUND;
const TTL = CONFIG.CACHE_TTL.SOUND;

export const soundApi = {
  async get() {
    const res = await api.get(EP, TTL);
    store.set('sound', res.data || CONFIG.DEFAULT_SOUND);
    return res.data;
  },

  async update(settings) {
    const res = await api.post(EP, settings);
    store.set('sound', { ...store.get('sound'), ...settings });
    cacheInvalidate(EP);
    return res.data;
  }
};
