import { CONFIG } from './config.js';
import { store } from './store.js';
import { router } from './router.js';
import { dashboardPage } from '../pages/dashboard.js';
import { todoPage } from '../pages/todo.js';
import { alarmPage } from '../pages/alarm.js';
import { schedulePage } from '../pages/schedule.js';
import { clockSettingsPage } from '../pages/clock-settings.js';
import { displaySettingsPage } from '../pages/display-settings.js';
import { soundSettingsPage } from '../pages/sound-settings.js';
import { wifiPage } from '../pages/wifi.js';
import { devicePage } from '../pages/device.js';

function registerPages() {
  router.register('dashboard', dashboardPage);
  router.register('todo', todoPage);
  router.register('alarm', alarmPage);
  router.register('schedule', schedulePage);
  router.register('clock-settings', clockSettingsPage);
  router.register('display-settings', displaySettingsPage);
  router.register('sound-settings', soundSettingsPage);
  router.register('wifi', wifiPage);
  router.register('device', devicePage);
}

function setupNavLinks() {
  document.querySelectorAll('.nav-item').forEach(link => {
    link.addEventListener('click', (e) => {
      e.preventDefault();
      const route = link.getAttribute('href').slice(1);
      router.navigateTo(route);
    });
  });
}

function setupOnlineOffline() {
  window.addEventListener('online', () => store.set('online', true));
  window.addEventListener('offline', () => store.set('online', false));

  store.subscribe('online', (online) => {
    const banner = document.getElementById('offline-banner');
    if (banner) {
      banner.classList.toggle('hidden', online);
    }
  });
}

function init() {
  registerPages();
  setupNavLinks();
  setupOnlineOffline();
  router.init();
}

if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', init);
} else {
  init();
}
