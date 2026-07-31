import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { displayApi } from '../api/display.js';
import { showToast } from '../components/toast.js';
import { createSlider } from '../components/slider.js';
import { createToggle } from '../components/toggle.js';
import { createSelect } from '../components/select.js';

let container = null;
let unsubscribers = [];

function render() {
  if (!container) return;
  clearChildren(container);

  const display = store.get('display');
  let brightness = display.brightness ?? 80;
  let autoDim = display.autoDim ?? false;
  let timeout = display.timeout ?? 30;
  let animation = display.animation ?? true;

  const brightnessSlider = createSlider({
    label: 'Brightness',
    value: brightness,
    min: 5,
    max: 100,
    onChange: async (v) => {
      brightness = v;
      await save({ brightness: v });
    }
  });

  const autoDimToggle = createToggle({
    label: 'Auto Dim',
    checked: autoDim,
    onChange: async (v) => {
      autoDim = v;
      await save({ autoDim: v });
    }
  });

  const timeoutSelect = createSelect({
    label: 'Screen Timeout',
    options: [
      { value: '10', label: '10 seconds' },
      { value: '30', label: '30 seconds' },
      { value: '60', label: '1 minute' },
      { value: '120', label: '2 minutes' },
      { value: '300', label: '5 minutes' },
      { value: '0', label: 'Never' }
    ],
    value: String(timeout),
    onChange: async (v) => {
      timeout = Number(v);
      await save({ timeout: timeout });
    }
  });

  const animToggle = createToggle({
    label: 'Animations',
    checked: animation,
    onChange: async (v) => {
      animation = v;
      await save({ animation: v });
    }
  });

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Display Settings' })
    ]),
    el('div', { class: 'card' }, [
      brightnessSlider,
      autoDimToggle,
      timeoutSelect,
      animToggle
    ])
  ]));
}

async function save(settings) {
  try {
    await displayApi.update(settings);
  } catch (e) {
    showToast('Failed to save display settings', 'error');
  }
}

async function load() {
  try { await displayApi.get(); } catch (e) { showToast('Failed to load display settings', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('display', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const displaySettingsPage = { init, load, render, destroy };
