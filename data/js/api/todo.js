import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.TODO;
const TTL = CONFIG.CACHE_TTL.TODOS;

export const todoApi = {
  async getAll() {
    const res = await api.get(EP, TTL);
    store.set('todos', res.data || []);
    return res.data;
  },

  async create(todo) {
    const res = await api.post(EP, { ...CONFIG.DEFAULT_TODO, ...todo });
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

  async toggleComplete(id, completed) {
    return this.update(id, { completed });
  }
};
