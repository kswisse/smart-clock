import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { deviceApi } from '../api/device.js';
import { showToast } from '../components/toast.js';

let container = null;
let unsubscribers = [];

function getBatteryColor(level) {
  if (!level) return 'var(--text-muted)';
  if (level > 60) return 'var(--success)';
  if (level > 20) return 'var(--warning)';
  return 'var(--danger)';
}

function render() {
  if (!container) return;
  clearChildren(container);

  const device = store.get('device');
  const batteryLevel = device.battery || 0;

  const info = el('div', { class: 'card card-info' }, [
    el('div', { class: 'info-row' }, [
      el('span', { class: 'info-label', text: 'Firmware' }),
      el('span', { class: 'info-value', text: device.firmware || 'Unknown' })
    ]),
    el('div', { class: 'info-row' }, [
      el('span', { class: 'info-label', text: 'ESP32 Chip' }),
      el('span', { class: 'info-value', text: device.chip || 'Unknown' })
    ]),
    el('div', { class: 'info-row' }, [
      el('span', { class: 'info-label', text: 'Flash Size' }),
      el('span', { class: 'info-value', text: device.flash || 'Unknown' })
    ]),
    el('div', { class: 'info-row' }, [
      el('span', { class: 'info-label', text: 'Free Heap' }),
      el('span', { class: 'info-value', text: device.heap || 'Unknown' })
    ]),
    el('div', { class: 'info-row' }, [
      el('span', { class: 'info-label', text: 'MAC Address' }),
      el('span', { class: 'info-value', text: device.mac || 'Unknown' })
    ])
  ]);

  const batteryCard = el('div', { class: 'card' }, [
    el('h3', { class: 'card-title mb-md', text: 'Battery' }),
    el('div', { class: 'flex items-center gap-md' }, [
      el('div', { class: 'battery-bar', style: { flex: '1' } }, [
        el('div', {
          class: 'battery-bar-fill',
          style: { width: `${batteryLevel}%`, background: getBatteryColor(batteryLevel) }
        })
      ]),
      el('span', { class: 'text-accent', text: `${batteryLevel}%` })
    ]),
    el('p', { class: 'text-sm text-muted mt-sm', text: device.battery ? `${(device.battery * 3.7 / 100).toFixed(2)}V estimated` : 'No battery data' })
  ]);

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Device Info' })
    ]),
    info,
    batteryCard
  ]));
}

async function load() {
  try { await deviceApi.get(); } catch (e) { showToast('Failed to load device info', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('device', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const devicePage = { init, load, render, destroy };
