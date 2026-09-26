import { CONFIG } from './config.js';
import { store } from './store.js';
import { $, $$ } from '../utils/dom.js';

const pages = {};
let currentPage = null;
let currentRoute = null;

export const router = {
  register(name, pageModule) {
    pages[name] = pageModule;
  },

  init() {
    window.addEventListener('hashchange', () => this._handleRoute());
    this._handleRoute();
  },

  navigateTo(route) {
    window.location.hash = route;
  },

  getCurrentRoute() {
    return currentRoute;
  },

  _handleRoute() {
    const hash = window.location.hash.slice(1) || 'dashboard';
    const route = CONFIG.ROUTES[hash];

    if (!route) {
      this.navigateTo('dashboard');
      return;
    }

    if (currentRoute === hash) return;

    this._mountPage(hash, route);
  },

  _mountPage(route, routeConfig) {
    const container = $('#page-container');
    if (!container) return;

    if (currentPage && currentPage.destroy) {
      currentPage.destroy();
    }

    container.innerHTML = '';

    const pageName = routeConfig.page;
    const pageModule = pages[pageName];

    if (!pageModule) {
      container.innerHTML = `<div class="empty-state"><p>Page "${pageName}" not found</p></div>`;
      currentPage = null;
      currentRoute = route;
      return;
    }

    if (pageModule.init) {
      pageModule.init(container);
    }

    if (pageModule.load) {
      pageModule.load();
    }

    if (pageModule.render) {
      pageModule.render();
    }

    currentPage = pageModule;
    currentRoute = route;
    store.set('currentPage', route);

    this._updateNavActive(route);
    this._updateTitle(routeConfig.title);
  },

  _updateNavActive(route) {
    $$('.nav-item').forEach(item => {
      const page = item.dataset.page;
      const isActive = page === route || (route === 'dashboard' && page === 'dashboard');
      item.classList.toggle('active', isActive);
    });
  },

  _updateTitle(title) {
    document.title = `${title} - ESP32 Smart Clock`;
  }
};
