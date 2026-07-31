import { el, clearChildren } from '../utils/dom.js';
import { store } from '../core/store.js';
import { scheduleApi } from '../api/schedule.js';
import { showToast } from '../components/toast.js';
import { confirmDialog } from '../components/dialog.js';
import { showModal } from '../components/modal.js';
import { createInput } from '../components/input.js';
import { createSelect } from '../components/select.js';
import { CONFIG } from '../core/config.js';

let container = null;
let currentDay = new Date().getDay();
let unsubscribers = [];

function getEntriesByDay(day) {
  return (store.get('schedule') || []).filter(s => s.day === day);
}

function renderDayTab(day, index) {
  return el('button', {
    class: `btn btn-sm ${index === currentDay ? 'btn-primary' : 'btn-ghost'}`,
    text: CONFIG.DAYS[index],
    onClick: () => { currentDay = index; render(); }
  });
}

function renderEntry(entry) {
  const delBtn = el('button', {
    class: 'btn btn-icon btn-ghost btn-sm',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M6 19c0 1.1.9 2 2 2h8c1.1 0 2-.9 2-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>',
    onClick: (e) => { e.stopPropagation(); deleteEntry(entry); }
  });

  return el('div', {
    class: 'schedule-entry',
    style: { borderLeftColor: entry.color || 'var(--accent)' }
  }, [
    el('div', { class: 'schedule-time', text: `${entry.start} - ${entry.end}` }),
    el('div', { class: 'schedule-title', text: entry.title || 'Untitled' }),
    delBtn
  ]);
}

async function deleteEntry(entry) {
  const ok = await confirmDialog('Delete Entry', `Delete "${entry.title}"?`);
  if (!ok) return;
  try {
    await scheduleApi.remove(entry.id);
    showToast('Entry deleted', 'success');
  } catch (e) {
    showToast('Failed to delete', 'error');
  }
}

function openAddModal() {
  let title = '', start = '09:00', end = '10:00', color = CONFIG.COLORS[0];

  const titleInput = createInput({ label: 'Title', value: title, placeholder: 'Class or event name', onInput: v => title = v });
  const startInput = createInput({ label: 'Start Time', type: 'time', value: start, onInput: v => start = v });
  const endInput = createInput({ label: 'End Time', type: 'time', value: end, onInput: v => end = v });

  const colorPicker = el('div', { class: 'color-picker mt-sm' });
  CONFIG.COLORS.forEach(c => {
    const dot = el('div', {
      class: `color-option ${c === color ? 'selected' : ''}`,
      style: { background: c },
      onClick: () => {
        color = c;
        colorPicker.querySelectorAll('.color-option').forEach(d => d.classList.remove('selected'));
        dot.classList.add('selected');
      }
    });
    colorPicker.appendChild(dot);
  });

  const daySelect = createSelect({
    label: 'Day',
    options: CONFIG.DAYS_FULL.map((d, i) => ({ value: String(i), label: d })),
    value: String(currentDay),
    onChange: v => currentDay = Number(v)
  });

  const form = el('div', {}, [
    titleInput,
    daySelect,
    el('div', { class: 'flex gap-sm' }, [startInput, endInput]),
    el('div', { class: 'input-group' }, [
      el('label', { class: 'input-label', text: 'Color' }), colorPicker
    ])
  ]);

  showModal({
    title: 'Add Schedule Entry',
    content: form,
    onSave: async () => {
      if (!title.trim()) return;
      try {
        await scheduleApi.create({ day: currentDay, start, end, title: title.trim(), color });
        showToast('Entry added', 'success');
      } catch (e) {
        showToast('Failed to add entry', 'error');
      }
    },
    onCancel: () => {}
  });
}

function render() {
  if (!container) return;
  clearChildren(container);

  const entries = getEntriesByDay(currentDay);
  const tabs = el('div', { class: 'flex gap-sm mb-md flex-wrap' });
  CONFIG.DAYS.forEach((_, i) => tabs.appendChild(renderDayTab(_, i)));

  const list = el('div', {});
  if (entries.length === 0) {
    list.appendChild(el('div', { class: 'empty-state' }, [
      el('p', { text: `No entries for ${CONFIG.DAYS_FULL[currentDay]}` })
    ]));
  } else {
    entries.forEach(e => list.appendChild(renderEntry(e)));
  }

  const fab = el('button', {
    class: 'fab',
    html: '<svg viewBox="0 0 24 24"><path fill="currentColor" d="M19 13h-6v6h-2v-6H5v-2h6V5h2v6h6v2z"/></svg>',
    onClick: openAddModal
  });

  container.appendChild(el('div', { class: 'page-content' }, [
    el('div', { class: 'page-header' }, [
      el('h1', { class: 'page-title', text: 'Schedule' })
    ]),
    tabs,
    list,
    fab
  ]));
}

async function load() {
  try { await scheduleApi.getAll(); } catch (e) { showToast('Failed to load schedule', 'error'); }
}

function init(c) {
  container = c;
  unsubscribers.push(store.subscribe('schedule', () => render()));
}

function destroy() {
  unsubscribers.forEach(unsub => unsub());
  unsubscribers = [];
  container = null;
}

export const schedulePage = { init, load, render, destroy };
