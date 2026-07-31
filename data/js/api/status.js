import { api } from './client.js';
import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';

const EP = CONFIG.ENDPOINTS.STATUS;
const TTL = CONFIG.CACHE_TTL.STATUS;

export const statusApi = {
  async get() {
    const res = await api.get(EP, TTL);
    const d = res.data || {};
    if (d.time) store.set('time', d.time);
    if (d.todos) store.set('todos', d.todos);
    if (d.alarms) store.set('alarms', d.alarms);
    if (d.schedule) store.set('schedule', d.schedule);
    if (d.display) store.set('display', d.display);
    if (d.sound) store.set('sound', d.sound);
    if (d.wifi) store.set('wifi', d.wifi);
    if (d.device) store.set('device', d.device);
    return d;
  }
};
