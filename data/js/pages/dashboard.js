import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { statusApi } from '../api/status.js';
import { timeApi } from '../api/time.js';
import { showToast } from '../components/toast.js';

let container = null;
let unsubscribers = [];

function renderStatusCards() {
  const wifi = store.get('wifi');
  const device = store.get('device');
  const alarms = store.get('alarms') || [];
  const todos = store.get('todos') || [];
  const nextAlarm = alarms.find(a => a.enabled);

  return el('div', { class: 'dashboard-grid' }, [
    createStatCard('WiFi', wifi.connected ? wifi.ssid : 'Disconnected', wifi.connected ? 'var(--success)' : 'var(--danger)'),
    createStatCard('Battery', device.battery ? `${device.battery}%` : '--', getBatteryColor(device.battery)),
    createStatCard('Alarms', alarms.length ? `${alarms.filter(a => a.enabled).length} active` : 'None', 'var(--accent)'),
    createStatCard('Todos', `${todos.filter(t => !t.completed).length} pending`, 'var(--warning)')
  ]);
}

function createStatCard(label, value, color) {
  return el('div', { class: 'stat-card' }, [
    el('div', { class: 'stat-value', text: value, style: { color } }),
    el('div', { class: 'stat-label', text: label })
  ]);
}

function getBatteryColor(level) {
  if (!level) return 'var(--text-muted)';
  if (level > 60) return 'var(--success)';
  if (level > 20) return 'var(--warning)';
  return 'var(--danger)';
}

function renderTime() {
  const time = store.get('time');
  return el('div', { class: 'dashboard-time' }, [
    el('div', { class: 'time', text: time.current || '--:--' }),
    el('div', { class: 'date', text: time.date || '---' })
  ]);
}

function renderActions() {
  return el('div', { class: 'flex gap-sm mt-md', style: { justifyContent: 'center' } }, [
    el('button', { class: 'btn btn-ghost btn-sm', text: 'Sync Time', onClick: handleSyncTime }),
    el('button', { class: 'btn btn-ghost btn-sm', text: 'Refresh', onClick: load })
  ]);
}

async function handleSyncTime() {
  try {
    await timeApi.get();
    showToast('Time synced', 'success');
  } catch (e) {
    showToast('Sync failed: ' + e.message, 'error');
  }
}

async function load() {
  try {
    await statusApi.get();
  } catch (e) {
    showToast('Failed to load status', 'error');
  }
}

function render() {
  if (!container) return;
  clearChildren(container);

  const page = el('div', { class: 'page-content' }, [
    renderTime(),
    renderStatusCards(),
    renderActions()
  ]);
  container.appendChild(page);
}

function init(c) {
  container = c;
  unsubscribers.push(
    store.subscribe('time', () => render()),
    store.subscribe('wifi', () => render()),
    store.subscribe('device', () => render()),
    store.subscribe('alarms', () => render()),
    store.subscribe('todos', () => render())
  );
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const dashboardPage = { init, load, render, destroy };
