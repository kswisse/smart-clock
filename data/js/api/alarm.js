import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.ALARM;
const TTL = CONFIG.CACHE_TTL.ALARMS;

export const alarmApi = {
  async getAll() {
    const res = await api.get(EP, TTL);
    store.set('alarms', res.data || []);
    return res.data;
  },

  async create(alarm) {
    const res = await api.post(EP, { ...CONFIG.DEFAULT_ALARM, ...alarm });
    await this.getAll();
    return res.data;
  },

  async update(id, updates) {
    const res = await api.put(`${EP}/${id}`, updates);
    await this.getAll();
    return res.data;
  },

  async remove(id) {
    await api.delete(`${EP}/${id}`);
    await this.getAll();
  },

  async toggleEnabled(id, enabled) {
    return this.update(id, { enabled });
  }
};
