import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { wifiApi } from '../api/wifi.js';
import { showToast } from '../components/toast.js';
import { createInput } from '../components/input.js';
import { showModal } from '../components/modal.js';

let container = null;
let unsubscribers = [];

function signalIcon(rssi) {
  const strength = rssi > -50 ? 3 : rssi > -70 ? 2 : 1;
  const bars = [1, 2, 3].map(h =>
    `<rect x="${(h - 1) * 5}" y="${12 - h * 3}" width="4" height="${h * 3}" fill="${h <= strength ? 'var(--accent)' : 'var(--text-muted)'}" rx="1"/>`
  ).join('');
  return `<svg width="20" height="16" viewBox="0 0 16 16">${bars}</svg>`;
}

function renderNetworkItem(network) {
  const lockIcon = network.secure ? '<span class="wifi-lock">&#128274;</span>' : '';

  return el('div', {
    class: 'wifi-network',
    onClick: () => openConnectModal(network.ssid)
  }, [
    el('div', { class: 'wifi-signal', html: signalIcon(network.rssi) }),
    el('div', { class: 'wifi-name', text: network.ssid }),
    lockIcon ? el('div', { html: lockIcon }) : null
  ].filter(Boolean));
}

function openConnectModal(ssid) {
  let password = '';
  const passInput = createInput({
    label: 'Password',
    type: 'password',
    placeholder: 'Enter WiFi password',
    onInput: v => password = v
  });

  showModal({
    title: `Connect to ${ssid}`,
    content: el('div', {}, [
      el('p', { class: 'text-sm text-muted mb-md', text: `Connecting to: ${ssid}` }),
      passInput
    ]),
    onSave: async () => {
      try {
        await wifiApi.connect(ssid, password);
        showToast(`Connected to ${ssid}`, 'success');
      } catch (e) {
        showToast('Connection failed: ' + e.message, 'error');
      }
    },
    onCancel: () => {}
  });
}

function render() {
  if (!container) return;
  clearChildren(container);

  const wifi = store.get('wifi');
  const networks = wifi.networks || [];

  const statusCard = el('div', { class: 'card' }, [
    el('div', { class: 'flex items-center justify-between mb-md' }, [
      el('h3', { text: 'Status' }),
      el('span', {
        class: `badge ${wifi.connected ? 'badge-success' : 'badge-danger'}`,
        text: wifi.connected ? 'Connected' : 'Disconnected'
      })
    ]),
    wifi.connected ? el('div', { class: 'card-info' }, [
      el('div', { class: 'info-row' }, [
        el('span', { class: 'info-label', text: 'SSID' }),
        el('span', { class: 'info-value', text: wifi.ssid })
      ]),
      el('div', { class: 'info-row' }, [
        el('span', { class: 'info-label', text: 'IP' }),
        el('span', { class: 'info-value', text: wifi.ip })
      ]),
      el('div', { class: 'info-row' }, [
        el('span', { class: 'info-label', text: 'Signal' }),
        el('span', { class: 'info-value', text: `${wifi.rssi} dBm` })
      ])
    ]) : el('p', { class: 'text-muted', text: 'Not connected to any network' }),
    wifi.connected ? el('button', {
      class: 'btn btn-danger btn-block mt-md',
      text: 'Disconnect',
      onClick: async () => {
        try {
          await wifiApi.disconnect();
          showToast('Disconnected', 'success');
        } catch (e) {
          showToast('Failed to disconnect', 'error');
        }
      }
    }) : null
  ]);

  const scanBtn = el('button', {
    class: 'btn btn-ghost btn-block mb-md',
    text: networks.length ? 'Scan Again' : 'Scan Networks',
    onClick: async () => {
      try {
        await wifiApi.scan();
        showToast('Scan complete', 'success');
      } catch (e) {
        showToast('Scan failed', 'error');
      }
    }
  });

  const networkList = el('div', {});
  if (networks.length > 0) {
    networks.sort((a, b) => b.rssi - a.rssi);
    networks.forEach(n => networkList.appendChild(renderNetworkItem(n)));
  } else {
    networkList.appendChild(el('div', { class: 'empty-state' }, [
      el('p', { text: 'No networks found. Tap Scan to search.' })
    ]));
  }

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'WiFi Settings' })
    ]),
    statusCard,
    el('div', { class: 'section-title', text: 'Available Networks' }),
    scanBtn,
    networkList
  ]));
}

async function load() {
  try { await wifiApi.getStatus(); } catch (e) { showToast('Failed to load WiFi status', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('wifi', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const wifiPage = { init, load, render, destroy };
