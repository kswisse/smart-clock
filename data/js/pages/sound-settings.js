import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { soundApi } from '../api/sound.js';
import { showToast } from '../components/toast.js';
import { createSlider } from '../components/slider.js';
import { createSelect } from '../components/select.js';
import { CONFIG } from '../core/config.js';

let container = null;
let unsubscribers = [];

function render() {
  if (!container) return;
  clearChildren(container);

  const sound = store.get('sound');
  let volume = sound.alarmVolume ?? 50;
  let alarmSound = sound.alarmSound ?? 'default';

  const volSlider = createSlider({
    label: 'Alarm Volume',
    value: volume,
    min: 0,
    max: 100,
    onChange: async (v) => {
      volume = v;
      await save({ alarmVolume: v });
    }
  });

  const soundSelect = createSelect({
    label: 'Alarm Sound',
    options: CONFIG.ALARM_SOUNDS.map(s => ({ value: s, label: s.charAt(0).toUpperCase() + s.slice(1) })),
    value: alarmSound,
    onChange: async (v) => {
      alarmSound = v;
      await save({ alarmSound: v });
    }
  });

  const testBtn = el('button', {
    class: 'btn btn-ghost btn-block mt-md',
    text: 'Test Sound',
    onClick: () => showToast('Playing test sound...', 'info')
  });

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Sound Settings' })
    ]),
    el('div', { class: 'card' }, [
      volSlider,
      soundSelect,
      testBtn
    ])
  ]));
}

async function save(settings) {
  try {
    await soundApi.update(settings);
  } catch (e) {
    showToast('Failed to save sound settings', 'error');
  }
}

async function load() {
  try { await soundApi.get(); } catch (e) { showToast('Failed to load sound settings', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('sound', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const soundSettingsPage = { init, load, render, destroy };
