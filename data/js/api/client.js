import { CONFIG } from '../core/config.js';
import { store } from '../core/store.js';
import { cacheGet, cacheSet, cacheInvalidate } from '../state/cache.js';

class ApiClient {
  constructor() {
    this._queue = [];
    this._processing = false;
  }

  async request(method, endpoint, body = null, cacheTTL = 0, useCache = true) {
    if (useCache && method === 'GET' && cacheTTL > 0) {
      const cached = cacheGet(endpoint);
      if (cached !== null) return cached;
    }

    const opts = {
      method,
      headers: { 'Content-Type': 'application/json' }
    };
    if (body) opts.body = JSON.stringify(body);

    try {
      const controller = new AbortController();
      const timeout = setTimeout(() => controller.abort(), CONFIG.API_TIMEOUT);
      opts.signal = controller.signal;

      const res = await fetch(CONFIG.API_BASE + endpoint, opts);
      clearTimeout(timeout);

      if (!res.ok) {
        const text = await res.text().catch(() => '');
        throw new ApiError(res.status, text || res.statusText);
      }

      const json = await res.json();
      if (json.success === false) {
        throw new ApiError(400, json.message || 'Request failed');
      }

      if (method === 'GET' && cacheTTL > 0) {
        cacheSet(endpoint, json, cacheTTL);
      }
      if (method !== 'GET') {
        cacheInvalidate(endpoint);
        this._invalidateRelated(endpoint);
      }

      store.set('online', true);
      return json;
    } catch (err) {
      if (err.name === 'AbortError') {
        throw new ApiError(408, 'Request timeout');
      }
      if (err instanceof ApiError) throw err;
      store.set('online', false);
      throw new ApiError(0, err.message || 'Network error');
    }
  }

  _invalidateRelated(endpoint) {
    if (endpoint.startsWith('/api/todo')) cacheInvalidate('/api/status');
    if (endpoint.startsWith('/api/alarm')) cacheInvalidate('/api/status');
    if (endpoint.startsWith('/api/schedule')) cacheInvalidate('/api/status');
    if (endpoint.startsWith('/api/time')) cacheInvalidate('/api/status');
    if (endpoint.startsWith('/api/display')) cacheInvalidate('/api/status');
    if (endpoint.startsWith('/api/sound')) cacheInvalidate('/api/status');
    if (endpoint.startsWith('/api/wifi')) cacheInvalidate('/api/status');
  }

  get(endpoint, cacheTTL = 0, useCache = true) {
    return this.request('GET', endpoint, null, cacheTTL, useCache);
  }

  post(endpoint, body) {
    return this.request('POST', endpoint, body);
  }

  put(endpoint, body) {
    return this.request('PUT', endpoint, body);
  }

  delete(endpoint) {
    return this.request('DELETE', endpoint);
  }
}

export class ApiError extends Error {
  constructor(status, message) {
    super(message);
    this.name = 'ApiError';
    this.status = status;
  }
}

export const api = new ApiClient();
