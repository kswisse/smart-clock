import { CONFIG } from '../core/config.js';

const cache = new Map();

export function cacheGet(key) {
  const entry = cache.get(key);
  if (!entry) return null;
  if (entry.ttl > 0 && Date.now() - entry.ts > entry.ttl) {
    cache.delete(key);
    return null;
  }
  return entry.data;
}

export function cacheSet(key, data, ttl = 0) {
  cache.set(key, { data, ts: Date.now(), ttl });
}

export function cacheInvalidate(key) {
  if (key) cache.delete(key);
  else cache.clear();
}

export function cacheHas(key) {
  const entry = cache.get(key);
  if (!entry) return false;
  if (entry.ttl > 0 && Date.now() - entry.ts > entry.ttl) {
    cache.delete(key);
    return false;
  }
  return true;
}

export function cacheKeys() {
  return [...cache.keys()];
}
