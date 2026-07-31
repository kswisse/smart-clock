import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.WEATHER;
const TTL = CONFIG.CACHE_TTL.WEATHER;

export const weatherApi = {
  async get() {
    const res = await api.get(EP, TTL);
    store.set('weather', res.data || {});
    return res.data;
  },

  async refresh() {
    const res = await api.post(EP, {});
    store.set('weather', res.data || {});
    return res.data;
  }
};
