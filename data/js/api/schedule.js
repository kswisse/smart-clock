import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.SCHEDULE;
const TTL = CONFIG.CACHE_TTL.SCHEDULE;

export const scheduleApi = {
  async getAll() {
    const res = await api.get(EP, TTL);
    store.set('schedule', res.data || []);
    return res.data;
  },

  async create(entry) {
    const res = await api.post(EP, { ...CONFIG.DEFAULT_SCHEDULE, ...entry });
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

  getByDay(day) {
    return store.get('schedule').filter(s => s.day === day);
  }
};
