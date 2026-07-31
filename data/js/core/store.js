import { CONFIG } from './config.js';

class Store {
  constructor() {
    this._state = this._getDefaultState();
    this._listeners = new Map();
    this._globalListeners = [];
  }

  _getDefaultState() {
    return {
      currentPage: CONFIG.PAGES.DASHBOARD,
      loading: false,
      online: navigator.onLine,
      time: { current: '', date: '', timezone: '', mode: 'ntp' },
      todos: [],
      alarms: [],
      schedule: [],
      display: { ...CONFIG.DEFAULT_DISPLAY },
      sound: { ...CONFIG.DEFAULT_SOUND },
      wifi: { connected: false, ssid: '', ip: '', rssi: 0, networks: [] },
      device: { firmware: '', chip: '', flash: '', heap: '', mac: '', battery: 0 },
      weather: { temp: 0, feelsLike: 0, humidity: 0, condition: '', description: '', wind: 0, location: '', country: '', lastUpdate: 0 }
    };
  }

  get state() {
    return this._state;
  }

  set(key, value) {
    const keys = key.split('.');
    let target = this._state;
    for (let i = 0; i < keys.length - 1; i++) {
      target = target[keys[i]];
    }
    const lastKey = keys[keys.length - 1];
    const old = target[lastKey];
    if (old === value) return;
    target[lastKey] = value;
    this._notify(key, value, old);
  }

  get(key) {
    return key.split('.').reduce((o, k) => o?.[k], this._state);
  }

  update(key, updater) {
    const current = this.get(key);
    this.set(key, updater(current));
  }

  subscribe(key, callback) {
    if (!this._listeners.has(key)) this._listeners.set(key, new Set());
    this._listeners.get(key).add(callback);
    return () => this._listeners.get(key)?.delete(callback);
  }

  subscribeAll(callback) {
    this._globalListeners.push(callback);
    return () => {
      this._globalListeners = this._globalListeners.filter(cb => cb !== callback);
    };
  }

  _notify(key, value, old) {
    this._listeners.get(key)?.forEach(cb => cb(value, old));
    this._globalListeners.forEach(cb => cb(key, value, old));
  }

  reset() {
    this._state = this._getDefaultState();
    this._notify('*', this._state);
  }
}

export const store = new Store();
