import { dashboardPage } from '../pages/dashboard.js';
import { todoPage } from '../pages/todo.js';
import { alarmPage } from '../pages/alarm.js';
import { schedulePage } from '../pages/schedule.js';
import { clockSettingsPage } from '../pages/clock-settings.js';
import { displaySettingsPage } from '../pages/display-settings.js';
import { soundSettingsPage } from '../pages/sound-settings.js';
import { wifiPage } from '../pages/wifi.js';
import { devicePage } from '../pages/device.js';

const pages = {
  dashboard: dashboardPage,
  todo: todoPage,
  alarm: alarmPage,
  schedule: schedulePage,
  'clock-settings': clockSettingsPage,
  'display-settings': displaySettingsPage,
  'sound-settings': soundSettingsPage,
  wifi: wifiPage,
  device: devicePage
};

let currentPage = null;
let container = null;

async function renderRoute() {
  const route = location.hash.slice(1) || 'dashboard';
  const nextPage = pages[route] || dashboardPage;
  currentPage?.destroy?.();
  container.replaceChildren();
  currentPage = nextPage;
  currentPage.init(container);
  currentPage.render();
  await currentPage.load();

  document.querySelectorAll('.nav-item').forEach(item => {
    const target = item.getAttribute('href')?.slice(1);
    const settingsRoute = route.endsWith('-settings');
    item.classList.toggle('active', target === route || (settingsRoute && target === 'clock-settings'));
  });
}

export const router = {
  start(target) {
    container = target;
    window.addEventListener('hashchange', renderRoute);
    renderRoute();
  },

  navigateTo(route) {
    location.hash = route;
  }
};
