import { router } from './router.js';
import { store } from './store.js';
import { initNavbar } from '../components/navbar.js';

function updateConnectionBanner() {
  const banner = document.querySelector('#offline-banner');
  banner?.classList.toggle('hidden', store.get('online') !== false);
}

function boot() {
  const container = document.querySelector('#page-container');
  if (!container) return;

  initNavbar();
  store.subscribe('online', updateConnectionBanner);
  window.addEventListener('online', () => store.set('online', true));
  window.addEventListener('offline', () => store.set('online', false));
  updateConnectionBanner();
  router.start(container);
}

if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', boot, { once: true });
} else {
  boot();
}
