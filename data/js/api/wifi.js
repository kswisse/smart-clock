import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.WIFI;
const TTL = CONFIG.CACHE_TTL.WIFI;

export const wifiApi = {
  async getStatus() {
    const res = await api.get(EP, TTL, false);
    store.set('wifi', res.data || {});
    return res.data;
  },

  async scan() {
    const res = await api.get(EP + '?scan=1', 0, false);
    const current = store.get('wifi');
    store.set('wifi', { ...current, networks: res.data?.networks || [] });
    return res.data;
  },

  async connect(ssid, password) {
    const res = await api.post(EP, { ssid, password });
    await this.getStatus();
    return res.data;
  },

  async disconnect() {
    const res = await api.post(EP, { action: 'disconnect' });
    await this.getStatus();
    return res.data;
  }
};
