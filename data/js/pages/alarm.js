import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { alarmApi } from '../api/alarm.js';
import { showToast } from '../components/toast.js';
import { confirmDialog } from '../components/dialog.js';
import { showModal } from '../components/modal.js';
import { createNumberInput } from '../components/input.js';
import { createToggle } from '../components/toggle.js';
import { createSelect } from '../components/select.js';
import { createSlider } from '../components/slider.js';
import { CONFIG } from '../core/config.js';
import { padZero } from '../utils/helpers.js';

let container = null;
let unsubscribers = [];

function renderAlarmItem(alarm) {
  const timeStr = `${padZero(alarm.hour)}:${padZero(alarm.minute)}`;

  const toggleEl = createToggle({
    checked: alarm.enabled,
    onChange: (val) => toggleAlarm(alarm.id, val)
  });

  const days = el('div', { class: 'alarm-days' });
  CONFIG.DAYS.forEach((day, i) => {
    const isActive = alarm.repeat?.includes(i);
    days.appendChild(el('div', {
      class: `alarm-day ${isActive ? 'active' : ''}`,
      text: day.charAt(0)
    }));
  });

  const delBtn = el('button', {
    class: 'btn btn-icon btn-ghost btn-sm',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>',
    onClick: (e) => { e.stopPropagation(); deleteAlarm(alarm); }
  });

  return el('div', { class: 'alarm-item' }, [
    el('div', { class: 'alarm-time', text: timeStr }),
    el('div', { class: 'alarm-info' }, [
      days,
      el('div', { class: 'text-sm text-muted mt-sm', text: `Sound: ${alarm.sound || 'default'} | Vol: ${alarm.volume || 50}%` })
    ]),
    toggleEl,
    delBtn
  ]);
}

async function toggleAlarm(id, enabled) {
  try {
    await alarmApi.toggleEnabled(id, enabled);
    showToast(enabled ? 'Alarm enabled' : 'Alarm disabled', 'success');
  } catch (e) {
    showToast('Failed to update alarm', 'error');
  }
}

async function deleteAlarm(alarm) {
  const ok = await confirmDialog('Delete Alarm', `Delete alarm at ${padZero(alarm.hour)}:${padZero(alarm.minute)}?`);
  if (!ok) return;
  try {
    await alarmApi.remove(alarm.id);
    showToast('Alarm deleted', 'success');
  } catch (e) {
    showToast('Failed to delete', 'error');
  }
}

function openAddModal() {
  let hour = 7, minute = 0;
  let repeat = [];
  let sound = 'default';
  let volume = 50;

  const hourInput = createNumberInput({ label: 'Hour', value: hour, min: 0, max: 23, onInput: v => hour = v });
  const minuteInput = createNumberInput({ label: 'Minute', value: minute, min: 0, max: 59, onInput: v => minute = v });

  const daysRow = el('div', { class: 'repeat-days' });
  CONFIG.DAYS.forEach((day, i) => {
    const btn = el('button', {
      class: 'repeat-day-btn',
      text: day.charAt(0),
      onClick: () => {
        if (repeat.includes(i)) {
          repeat = repeat.filter(d => d !== i);
          btn.classList.remove('active');
        } else {
          repeat.push(i);
          btn.classList.add('active');
        }
      }
    });
    daysRow.appendChild(btn);
  });

  const soundSelect = createSelect({
    label: 'Alarm Sound',
    options: CONFIG.ALARM_SOUNDS.map(s => ({ value: s, label: s })),
    value: sound,
    onChange: v => sound = v
  });

  const volSlider = createSlider({ label: 'Volume', value: volume, min: 0, max: 100, onChange: v => volume = v });

  const form = el('div', {}, [
    el('div', { class: 'time-input-group mb-md' }, [hourInput, el('span', { class: 'time-separator', text: ':' }), minuteInput]),
    el('div', { class: 'input-group' }, [
      el('label', { class: 'input-label', text: 'Repeat Days' }), daysRow
    ]),
    soundSelect,
    volSlider
  ]);

  showModal({
    title: 'Add Alarm',
    content: form,
    onSave: async () => {
      try {
        await alarmApi.create({ hour, minute, repeat, sound, volume });
        showToast('Alarm added', 'success');
      } catch (e) {
        showToast('Failed to add alarm', 'error');
      }
    },
    onCancel: () => {}
  });
}

function render() {
  if (!container) return;
  clearChildren(container);

  const alarms = store.get('alarms') || [];
  const list = el('div', {});

  if (alarms.length === 0) {
    list.appendChild(el('div', { class: 'empty-state' }, [
      el('p', { text: 'No alarms set' })
    ]));
  } else {
    alarms.forEach(a => list.appendChild(renderAlarmItem(a)));
  }

  const fab = el('button', {
    class: 'fab',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M19 13h-6v6h-2v-6H5v-2h6V5h2v6h6v2z"/></svg>',
    onClick: openAddModal
  });

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Alarms' }),
      el('span', { class: 'badge badge-info', text: `${alarms.length}` })
    ]),
    list,
    fab
  ]));
}

async function load() {
  try { await alarmApi.getAll(); } catch (e) { showToast('Failed to load alarms', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('alarms', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const alarmPage = { init, load, render, destroy };
